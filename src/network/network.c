#include "network.h"
#include "../logging/logging.h"
#include "../core/vector.h"

#include <stddef.h>
#include <uv.h>
#include <jansson.h>

#define CABOR_SERVER_PORT 4120
#define CABOR_SERVER_BACKLOG 128
#define CABOR_IDLE_TIMEOUT_MS 60000

// For each concurrent tcp connection we allocate
// cabor_tcp_timeout and cabor_tcp_client

struct cabor_tcp_timeout;
typedef struct cabor_tcp_timeout cabor_tcp_timeout;

struct cabor_tcp_client;
typedef struct cabor_tcp_client cabor_tcp_client;

struct cabor_tcp_timeout 
{
    uv_timer_t handle;
    cabor_tcp_client* client;
};

struct cabor_tcp_client
{
    uv_tcp_t handle;
    cabor_tcp_timeout* timeout;
    cabor_vector data;
};

static void on_close_timeout(uv_handle_t* timeout)
{
    cabor_tcp_timeout* cabor_timeout = timeout->data;
    cabor_allocation alloc = {
        .mem = cabor_timeout,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
        .size = sizeof(cabor_tcp_timeout)
#endif
    };
    CABOR_FREE(&alloc);
}

static void on_close_tcp_client(uv_handle_t* client)
{
    cabor_tcp_client* cabor_client = client->data;
    destroy_cabor_vector(&cabor_client->data);
    cabor_allocation alloc = {
        .mem  = cabor_client,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
        .size = sizeof(cabor_tcp_client)
#endif
    };
    CABOR_FREE(&alloc);
}

static void on_timeout(uv_timer_t* timeout)
{
    cabor_tcp_timeout* cabor_timeout = timeout->data;
    cabor_tcp_client* cabor_client = cabor_timeout->client;
    uv_tcp_t* client = &cabor_client->handle;

    CABOR_LOG("client timed out, closing connection.");
    uv_close(client, on_close_tcp_client);
    uv_close(timeout, on_close_timeout);
}

void on_write(uv_write_t* req, int status)
{
    if (status < 0)
    {
        CABOR_LOG_ERR_F("write error: %s", uv_strerror(status));
    }

    cabor_allocation alloc =
    {
        .mem = req,
        .size = sizeof(uv_write_t)
    };

    CABOR_FREE(&alloc);
}

static void alloc_buffer(uv_handle_t* client, size_t suggested_size, uv_buf_t* buf)
{
    cabor_tcp_client* cabor_client = client->data;
    size_t current_size = cabor_client->data.size;

    // Reserve enough space at the end of the vector for incoming data
    cabor_vector_reserve(&cabor_client->data, current_size + suggested_size);
    void* buffer_begin_addr = cabor_vector_peek_uchar(&cabor_client->data);

    buf->base = buffer_begin_addr;
    buf->len = suggested_size;
}

static void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
    cabor_tcp_client* cabor_client = client->data;
    cabor_tcp_timeout* cabor_timeout = cabor_client->timeout;

    uv_timer_t* timeout = &cabor_timeout->handle;

    if (nread > 0)
    {
        cabor_client->data.size += nread;

        // Reset timer
        uv_timer_stop(timeout);
        uv_timer_start(timeout, on_timeout, CABOR_IDLE_TIMEOUT_MS, 0);
    }
    else if (nread < 0)
    {
        if (nread == UV_EOF)
        {
            CABOR_LOG("Received: EOF");
        }
        else if (nread == UV_ECONNRESET)
        {
            CABOR_LOG("Received: ECONNRESET");
        }
        else if (nread == UV_ENOBUFS)
        {
            CABOR_LOG("Received: ENOBUFS");
        }
        else if (nread == UV_EMFILE)
        {
            CABOR_LOG("Received: EMFILE");
        }

        // Reading data is completed
        cabor_vector_push_uchar(&cabor_client->data, '\0');

        double recieved_amount;
        const char* prefix = cabor_convert_bytes_to_human_readable(cabor_client->data.size, &recieved_amount);
        CABOR_LOG_F("data received %.3f %s", recieved_amount, prefix);

        cabor_network_request request;
        cabor_decode_network_request(cabor_client->data.vector_mem.mem, cabor_client->data.size, &request);

        cabor_allocation reqbuf = CABOR_MALLOC(sizeof(uv_write_t));
        uv_write_t* req = (uv_write_t*)reqbuf.mem;

        if (request.type == CABOR_COMPILE)
        {
            // run compiler (TODO) ... respond with program
            const char* program = "base64-encoded statically linked x86_64 program";

            cabor_network_response resp =
            {
                .program_text = program,
                .size = strlen(program),
                .error = false
            };

            cabor_allocation response_alloc;
            size_t response_size;

            cabor_encode_network_response(&resp, &response_alloc, &response_size);

            uv_buf_t wrbuf =
            {
                .base = response_alloc.mem,
                .len = response_alloc.size 
            };

            uv_write(req, client, &wrbuf, 1, on_write);

        }
        else if (request.type == CABOR_PING)
        {
            uv_buf_t wrbuf =
            {
                .base = NULL,
                .len = 0
            };

            uv_write(req, client, &wrbuf, 1, on_write);
        }
        else
        {
            CABOR_FREE(&req);
        }

        uv_close(timeout, on_close_timeout);
        uv_close(client, on_close_tcp_client);
    }
}

static void on_new_connection(uv_stream_t* server, int status) 
{
    cabor_server_context* ctx = server->data;

    if (status < 0) 
    {
        CABOR_LOG_ERR_F("New connection error: %s", uv_strerror(status));
        return;
    }

    cabor_allocation clientmem = CABOR_MALLOC(sizeof(cabor_tcp_client));
    cabor_tcp_client* cabor_client = clientmem.mem;
    cabor_client->data = cabor_create_vector(1024, CABOR_UCHAR, true);

    cabor_allocation timeoutmem = CABOR_MALLOC(sizeof(cabor_tcp_timeout));
    cabor_tcp_timeout* cabor_timeout = timeoutmem.mem;

    uv_timer_t* timeout = &cabor_timeout->handle;
    uv_tcp_t* client = &cabor_client->handle;
    uv_loop_t* loop = ctx->loopmem.mem;

    uv_timer_init(loop, timeout);
    uv_tcp_init(loop, client);

    cabor_timeout->client = cabor_client;
    cabor_client->timeout = cabor_timeout;

    timeout->data = cabor_timeout;
    client->data = cabor_client;

    if (uv_accept(server, client) == 0) 
    {
        CABOR_LOG("New client connected");
        uv_read_start(client, alloc_buffer, on_read);
        uv_timer_start(timeout, on_timeout, CABOR_IDLE_TIMEOUT_MS, 0);
    }
    else 
    {
        uv_close(client, on_close_tcp_client);
    }
}

int cabor_start_compile_server(cabor_server_context* ctx)
{
    ctx->loopmem = CABOR_MALLOC(sizeof(uv_loop_t));
    ctx->servermem = CABOR_MALLOC(sizeof(uv_tcp_t));

    uv_loop_t* loop = ctx->loopmem.mem;
    uv_loop_init(loop);

    uv_tcp_t* server = ctx->servermem.mem;
    uv_tcp_init(loop, server);
    server->data = ctx;

    struct sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", CABOR_SERVER_PORT, &addr);
    uv_tcp_bind(server, &addr, 0);

    int r = uv_listen(server, CABOR_SERVER_BACKLOG, on_new_connection);
    if (r)
    {
        CABOR_LOG_ERR_F("Listen error: %s", uv_strerror(r));
        return 1;
    }

    CABOR_LOG_F("TCP server running on port %d", CABOR_SERVER_PORT);
    uv_run(loop, UV_RUN_DEFAULT);

    uv_loop_close(loop);

    return 0;
}

int cabor_decode_network_request(const void* buffer, const size_t buffer_size, cabor_network_request* request)
{
    // TODO: provide jansson custom allocator

    json_t* root;
    json_error_t error;

    root = json_loads(buffer, 0, &error);

    if (!root)
    {
        CABOR_LOG("Error parsing JSON");
        return 1;
    }

    const char* type = json_string_value(json_object_get(root, "command"));
    if (strcmp(type, "compile") == 0)
    {
        request->type = CABOR_COMPILE;

        const char* source = json_string_value(json_object_get(root, "code"));
        size_t sourcelen = strlen(source);

        request->source = CABOR_MALLOC(sourcelen);
        memcpy(request->source.mem, source, sourcelen);
        return 0;
    }
    else if (strcmp(type, "ping") == 0)
    {
        request->type = CABOR_PING;
        return 0;
    }
    return 1;
}

void cabor_encode_network_response(const cabor_network_response* response, cabor_allocation* alloc, size_t* buffer_size)
{
    json_t* root = json_object();

    if (!response->error)
    {
        json_object_set_new(root, "program", json_string(response->program_text));
    }
    else
    {
        json_object_set_new(root, "error", json_string(response->program_text));
    }

    char* json_str = json_dumps(root, JSON_INDENT(4));
    size_t jsonlen = strlen(json_str);

    *alloc = CABOR_MALLOC(jsonlen);
    memcpy(alloc->mem, json_str, jsonlen);

    json_decref(root);
    free(json_str);
}
