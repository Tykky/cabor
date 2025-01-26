#pragma once

#include "../cabor_defines.h"
#include "../core/memory.h"
#include <stdbool.h>

typedef struct
{
    cabor_allocation loopmem;
    cabor_allocation servermem;
} cabor_server_context;

typedef enum
{
    CABOR_PING,
    CABOR_COMPILE,
    CABOR_SHUTDOWN
} cabor_command_type;

typedef struct
{
    cabor_command_type type;
    cabor_allocation source;
    size_t source_size;
} cabor_network_request;

typedef struct
{
    char* program_text;
    size_t size;
    bool error; // error message is placed in program_text when there is a error
} cabor_network_response;

int cabor_start_compile_server(cabor_server_context* ctx);
int cabor_shutdown_compile_server(cabor_server_context* ctx);

int cabor_decode_network_request(const void* buffer, const size_t buffer_size, cabor_network_request* request);
void cabor_encode_network_response(const cabor_network_response* response, cabor_allocation* alloc, size_t* buffer_size);
