#include "network.h"
#include "../logging/logging.h"
#include "../core/vector.h"

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

    CABOR_LOG_F("client timed out, closing connection.");
    uv_close(client, on_close_tcp_client);
    uv_close(timeout, on_close_timeout);
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
        //CABOR_LOG_F("Received %zd bytes", nread);
        cabor_client->data.size += nread;

        // Reset timer
        uv_timer_stop(timeout);
        uv_timer_start(timeout, on_timeout, CABOR_IDLE_TIMEOUT_MS, 0);

        //cabor_allocation reqbuf = CABOR_MALLOC(sizeof(uv_write_t));
        //uv_write_t* req = (uv_write_t*)reqbuf.mem;
        //uv_buf_t wrbuf = uv_buf_init(buf->base, nread);
        //uv_write(req, client, &wrbuf, 1, NULL);
    }
    else if (nread < 0)
    {
        if (nread == UV_EOF)
        {
            CABOR_LOG_F("Received: EOF");
        }
        else if (nread == UV_ECONNRESET)
        {
            CABOR_LOG_F("Received: ECONNRESET");
        }
        else if (nread == UV_ENOBUFS)
        {
            CABOR_LOG_F("Received: ENOBUFS");
        }
        else if (nread == UV_EMFILE)
        {
            CABOR_LOG_F("Received: EMFILE");
        }

        // Reading data is completed
        cabor_vector_push_uchar(&cabor_client->data, '\0');

        double recieved_amount;
        const char* prefix = cabor_convert_bytes_to_human_readable(cabor_client->data.size, &recieved_amount);
        CABOR_LOG_F("data received %.3f %s", recieved_amount, prefix);

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

void cabor_decode_network_request(const void* buffer, const size_t buffer_size, cabor_network_request* request)
{
}

void cabor_encode_network_request(const cabor_network_response* response, void* buffer, size_t* buffer_size)
{
}
