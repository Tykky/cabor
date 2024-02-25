#pragma once

#include "../cabor_defines.h"

#include <stddef.h>

#define CABOR_MALLOC(size) cabor_malloc(get_global_cabor_allocator_context(), size)
#define CABOR_REALLOC(mem, size) cabor_realloc(get_global_cabor_allocator_context(), mem, size)
#define CABOR_CALLOC(num, size) cabor_calloc(get_global_cabor_allocator_context(), num, size)
#define CABOR_FREE(mem) cabor_free(get_global_cabor_allocator_context(), mem)

#define CABOR_CREATE_ALLOCATOR() create_cabor_allocator_context(get_global_cabor_allocator_context())
#define CABOR_DESTROY_ALLOCATOR() destroy_cabor_allocator_context(get_global_cabor_allocator_context())

#define CABOR_GET_ALLOCATED() cabor_get_current_allocated(get_global_cabor_allocator_context())

typedef struct
{
    void* mem;
    size_t size;
} cabor_allocation;

typedef struct 
{
    size_t allocated_mem;
} cabor_allocator_context;

void create_cabor_allocator_context  (cabor_allocator_context* alloc_ctx);
void destroy_cabor_allocator_context (cabor_allocator_context* alloc_ctx);

cabor_allocation cabor_malloc  (cabor_allocator_context* alloc_ctx, size_t size);
cabor_allocation cabor_realloc (cabor_allocator_context* alloc_ctx, cabor_allocation* old_alloc, size_t size);
cabor_allocation cabor_calloc  (cabor_allocator_context* alloc_ctx, size_t num, size_t size);
void             cabor_free    (cabor_allocator_context* alloc_ctx, cabor_allocation* alloc);

cabor_allocator_context* get_global_cabor_allocator_context();

size_t cabor_get_current_allocated(cabor_allocator_context* alloc_ctx);
const char* cabor_convert_bytes_to_human_readable(size_t bytes, double* converted);
