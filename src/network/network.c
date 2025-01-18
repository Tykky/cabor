#include "network.h"
#include "../logging/logging.h"

#include <uv.h>
#include <jansson.h>

#define CABOR_SERVER_PORT 4120
#define CABOR_SERVER_BACKLOG 128

typedef struct
{
    uv_tcp_t handle;
    size_t size;
} cabor_tcp_client;

void alloc_buffer(uv_handle_t* client, size_t suggested_size, uv_buf_t* buf)
{
    cabor_allocation mem = CABOR_MALLOC(suggested_size);
    buf->base = mem.mem;
    buf->len = suggested_size;
}

static void on_close(uv_handle_t* client)
{
    cabor_tcp_client* cabor_client = client->data;
    cabor_allocation alloc = {
        .mem = &cabor_client->handle,
        .size = cabor_client->size
    };
    CABOR_FREE(&alloc);
}

static void on_read(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
    cabor_tcp_client* cabor_client = client->data;
    if (nread > 0)
    {
        CABOR_LOG_F("Received %zd bytes %.*s\n", nread, (int)nread, buf->base);
        cabor_allocation reqbuf = CABOR_MALLOC(sizeof(uv_write_t));
        uv_write_t* req = (uv_write_t*)reqbuf.mem;
        uv_buf_t wrbuf = uv_buf_init(buf->base, buf->len);
        uv_write(req, client, &wrbuf, 1, NULL);
    }
    else if (nread < 0)
    {
        if (nread == UV_EOF)
        {
            CABOR_LOG_F("Received: EOF\n");
        }

        uv_close(client, on_close);
    }

    cabor_allocation bufalloc =
    {
        .mem = buf->base,
        .size = buf->len
    };

    CABOR_FREE(&bufalloc);
}

static void on_new_connection(uv_stream_t* server, int status) 
{
    cabor_server_context* ctx = (cabor_server_context*)server->data;

    if (status < 0) 
    {
        CABOR_LOG_ERR_F("New connection error: {}", uv_strerror(status));
        return;
    }

    cabor_allocation clientmem = CABOR_MALLOC(sizeof(cabor_tcp_client));
    cabor_tcp_client* cabor_client = clientmem.mem;
    cabor_client->size = sizeof(cabor_tcp_client);

    uv_tcp_t* client = &cabor_client->handle;
    client->data = cabor_client;

    uv_loop_t* loop = ctx->loopmem.mem;

    uv_tcp_init(loop, client);

    if (uv_accept(server, client) == 0) 
    {
        printf("New client connected\n");
        uv_read_start(client, alloc_buffer, on_read);
    }
    else 
    {
        uv_close((uv_handle_t*)client, on_close);
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
        CABOR_LOG_ERR_F("Listen error: %s\n", uv_strerror(r));
        return 1;
    }

    CABOR_LOG_F("TCP server running on port %d\n", CABOR_SERVER_PORT);
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
