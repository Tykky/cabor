#pragma once

#include "../core/memory.h"

typedef enum
{
    CABOR_PING,
    CABOR_COMPILE
} cabor_command_type;

typedef struct
{
    cabor_command_type type;
} cabor_network_request;

typedef struct
{
    char* json;
    size_t size;
} cabor_network_response;

void cabor_decode_network_request(void* buffer)
{
}
