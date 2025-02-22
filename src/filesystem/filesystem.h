#pragma once

#include "../core/memory.h"

typedef struct
{
    const char* filename;
    size_t size;
    cabor_allocation file_memory;
} cabor_file;

cabor_file* cabor_file_from_buffer(const char* buffer, size_t length);
cabor_file* cabor_load_file(const char* filename);
void cabor_dump_file_to_disk(cabor_file* file, const char* filename);
void cabor_destroy_file(cabor_file* file);
char cabor_read_byte_from_file(cabor_file* file, size_t idx);
