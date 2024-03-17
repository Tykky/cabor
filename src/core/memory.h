#pragma once

#include "../cabor_defines.h"

#include <stddef.h>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)
#define FUNC_STRING __func__

#define CABOR_MEMORY_DEBUG_STR __FILE__ ":" LINE_STRING "\n"

#define CABOR_MALLOC(size) cabor_malloc(get_global_cabor_allocator_context(), size, CABOR_MEMORY_DEBUG_STR)
#define CABOR_REALLOC(mem, size) cabor_realloc(get_global_cabor_allocator_context(), mem, size, CABOR_MEMORY_DEBUG_STR)
#define CABOR_CALLOC(num, size) cabor_calloc(get_global_cabor_allocator_context(), num, size, CABOR_MEMORY_DEBUG_STR)
#define CABOR_FREE(mem) cabor_free(get_global_cabor_allocator_context(), mem)

#define CABOR_CREATE_ALLOCATOR() create_cabor_allocator_context(get_global_cabor_allocator_context())
#define CABOR_DESTROY_ALLOCATOR() destroy_cabor_allocator_context(get_global_cabor_allocator_context())

#define CABOR_GET_ALLOCATED() cabor_get_current_allocated(get_global_cabor_allocator_context())
#define CABOR_GET_ALLOCATOR() get_global_cabor_allocator_context()

#define CABOR_MEMORY_DEBUG_ARRAY_SIZE 4096

typedef struct
{
    void* mem;
    size_t size;
} cabor_allocation;

typedef struct 
{
    size_t allocated_mem;
    char* debug[CABOR_MEMORY_DEBUG_ARRAY_SIZE];
    size_t debug_size;
} cabor_allocator_context;

void create_cabor_allocator_context  (cabor_allocator_context* alloc_ctx);
void destroy_cabor_allocator_context (cabor_allocator_context* alloc_ctx);

cabor_allocation cabor_malloc  (cabor_allocator_context* alloc_ctx, size_t size, const char* debug);
cabor_allocation cabor_realloc (cabor_allocator_context* alloc_ctx, cabor_allocation* old_alloc, size_t size, const char* debug);
cabor_allocation cabor_calloc  (cabor_allocator_context* alloc_ctx, size_t num, size_t size, const char* debug);
void             cabor_free    (cabor_allocator_context* alloc_ctx, cabor_allocation* alloc);

cabor_allocator_context* get_global_cabor_allocator_context();

size_t cabor_get_current_allocated(cabor_allocator_context* alloc_ctx);
const char* cabor_convert_bytes_to_human_readable(size_t bytes, double* converted);
