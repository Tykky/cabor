#include "network.h"
#include "../logging/logging.h"
#include "../core/vector.h"

#include <string.h>
#include <stddef.h>
#include <uv.h>
#include <jansson.h>

#ifdef _DEBUG && WIN32
#include <stdlib.h>
#include <crtdbg.h>k
#define malloc(s)       _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(c, s)    _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(p, s)   _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif


#define CABOR_SERVER_PORT 4120
#define CABOR_SERVER_BACKLOG 128
#define CABOR_IDLE_TIMEOUT_MS 1000

// The flow of the network requests go something like this:
//   1. cabor_start_compile_server() creates the server and begins the server loop
//   2. For each new concurrent tcp connection callback on_new_connection() is called
//   3. Connection starts sending data and callback on_read() is called when data is ready
//   4. On EOF we queue work to threadpool to handle the new request
//   5. When a thread picks up the work on_work() callback gets called on a worker thread
//      - Here we do the actual work, parse json, compile the program and prepare response buffer
//   6. When work is done on_after_work callback gets called and finally responds to the request
//
// Remarks: Environment variable UV_THREADPOOL_SIZE controls the amount of worker threads in the
// threadpool. It's recommend to set this to the amount of logical cpus on the machine for optimal perf


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
    cabor_vector* data;
    cabor_allocation response; // buffer that contains encoded json
    size_t response_size;
    bool shutdown_requested;
    cabor_server_context* server_context;
};

static void on_close_timeout(uv_handle_t* timeout)
{
    cabor_tcp_timeout* cabor_timeout = timeout->data;
    CABOR_DELETE(cabor_tcp_timeout, cabor_timeout);
}

static void on_close_tcp_client(uv_handle_t* client)
{
    cabor_tcp_client* cabor_client = client->data;
    cabor_destroy_vector(cabor_client->data);
    CABOR_DELETE(cabor_tcp_client, cabor_client);
}

static void on_timeout(uv_timer_t* timeout)
{
    cabor_tcp_timeout* cabor_timeout = timeout->data;
    cabor_tcp_client* cabor_client = cabor_timeout->client;
    uv_tcp_t* client = &cabor_client->handle;

    CABOR_LOG("client timed out, closing connection.");
    uv_close((uv_handle_t*)client, on_close_tcp_client);
    uv_close((uv_handle_t*)timeout, on_close_timeout);
}

void on_write(uv_write_t* req, int status)
{
    if (status < 0)
    {
        CABOR_LOG_ERR_F("write error: %s", uv_strerror(status));
    }

    cabor_tcp_client* client = req->data;

    if (client->response_size > 0)
    {
        cabor_allocation alloc =
        {
            .mem = client->response.mem,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .size = client->response_size
#endif
        };
        CABOR_FREE(&alloc);
    }

    CABOR_DELETE(uv_write_t, req);
}

static void alloc_buffer(uv_handle_t* client, size_t suggested_size, uv_buf_t* buf)
{
    cabor_tcp_client* cabor_client = client->data;
    size_t current_size = cabor_client->data->size;

    // Reserve enough space at the end of the vector for incoming data
    cabor_vector_reserve(cabor_client->data, current_size + suggested_size);
    void* buffer_begin_addr = cabor_vector_peek_uchar(cabor_client->data);

    buf->base = buffer_begin_addr;
    buf->len = suggested_size;
}

// Called from worker thread
static void on_work(uv_work_t* work)
{
    cabor_tcp_client* cabor_client = work->data;
    uv_tcp_t* client = &cabor_client->handle;

    cabor_vector_push_uchar(cabor_client->data, '\0');

    double recieved_amount;
    const char* prefix = cabor_convert_bytes_to_human_readable(cabor_client->data->size, &recieved_amount);
    CABOR_LOG_F("data received %.3f %s", recieved_amount, prefix);

    cabor_network_request request;
    cabor_decode_network_request(cabor_client->data->vector_mem.mem, cabor_client->data->size, &request);

    if (request.type == CABOR_COMPILE)
    {
        CABOR_LOG_F("Compile request: %.*s", request.source_size, request.source.mem);

        // run compiler (TODO) ... respond with program
        const char* program = "base64-encoded statically linked x86_64 program";

        // to simulate compiling
        int a = 1, b = 1, c = 1;
        for (size_t i = 1; i < 100000000; i++)
        {
            a *= i;
            b *= i;
            c *= i;
        }

        cabor_network_response resp =
        {
            .program_text = program,
            .size = strlen(program),
            .error = false
        };

        // Sending the data back happens in on_after_work
        cabor_encode_network_response(&resp, &cabor_client->response, &cabor_client->response_size);

    }
    else if (request.type == CABOR_PING)
    {
        cabor_client->response_size = 0;
    }
    else if (request.type == CABOR_SHUTDOWN)
    {
        cabor_client->response_size = 0;
        cabor_client->shutdown_requested = true;
    }

    if (request.source_size > 0)
    {
        CABOR_FREE(&request.source);
    }

}

void count_open_handles(uv_handle_t* handle, void* arg)
{
    int* open_handles = (int*)arg;
    (*open_handles)++;
}

// Called on main/loop thread after worker thread is done
static void on_after_work(uv_work_t* work, int status)
{
    cabor_tcp_client* cabor_client = work->data;
    uv_tcp_t* client = &cabor_client->handle;

    cabor_tcp_timeout* cabor_timeout = cabor_client->timeout;
    uv_timer_t* timeout = &cabor_timeout->handle;

    cabor_allocation reqbuf = CABOR_MALLOC(sizeof(uv_write_t));
    uv_write_t* req = (uv_write_t*)reqbuf.mem;
    req->data = cabor_client;

    uv_buf_t wrbuf =
    {
        .base = cabor_client->response_size > 0 ? cabor_client->response.mem : NULL,
        .len = cabor_client->response_size,
    };

    bool shutdown = cabor_client->shutdown_requested;

    uv_write(req, client, &wrbuf, 1, on_write);

    uv_close((uv_handle_t*)timeout, on_close_timeout);
    uv_close((uv_handle_t*)client, on_close_tcp_client);

    if (shutdown)
    {
        CABOR_DELETE(uv_work_t, work);
        uv_close((uv_handle_t*)cabor_client->server_context->servermem.mem, NULL);
        CABOR_LOG("SHUTDOWN received, shutting down the server")
        return;
    }

    CABOR_DELETE(uv_work_t, work);
}

static void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
    cabor_tcp_client* cabor_client = client->data;
    cabor_tcp_timeout* cabor_timeout = cabor_client->timeout;

    uv_timer_t* timeout = &cabor_timeout->handle;

    if (nread > 0)
    {
        cabor_client->data->size += nread;

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

        cabor_allocation work_alloc = CABOR_MALLOC(sizeof(uv_work_t));
        uv_work_t* work = work_alloc.mem;
        work->data = cabor_client;

        uv_queue_work(client->loop, work, on_work, on_after_work);
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

    CABOR_NEW(cabor_tcp_client, cabor_client);
    cabor_client->data = cabor_create_vector(2, CABOR_UCHAR, true);
    cabor_client->shutdown_requested = false;
    cabor_client->server_context = server->data;

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
        uv_close((uv_handle_t*)client, on_close_tcp_client);
        CABOR_DELETE(cabor_tcp_timeout, cabor_timeout);
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

    CABOR_DELETE(uv_loop_t, loop);
    CABOR_DELETE(uv_tcp_t, server);

    return 0;
}

static void* json_malloc(size_t size)
{
    return malloc(size);
}

static void json_free(void* ptr)
{
    free(ptr);
}

int cabor_decode_network_request(const void* buffer, const size_t buffer_size, cabor_network_request* request)
{
    json_set_alloc_funcs(json_malloc, json_free);

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
        request->source_size = sourcelen;
        json_decref(root);
        return 0;
    }
    else if (strcmp(type, "ping") == 0)
    {
        request->type = CABOR_PING;
        request->source_size = 0;
        json_decref(root);
        return 0;
    }
    else if (strcmp(type, "shutdown") == 0)
    {
        request->type = CABOR_SHUTDOWN;
        request->source_size = 0;
        json_decref(root);
        return 0;
    }
    json_decref(root);
    return 1;
}

void cabor_encode_network_response(const cabor_network_response* response, cabor_allocation* alloc, size_t* buffer_size)
{
    json_set_alloc_funcs(json_malloc, json_free);

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
    *buffer_size = jsonlen;

    json_decref(root);
    free(json_str);
}
