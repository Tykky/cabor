#include "memory.h"
#include "stdlib.h"

static cabor_allocator_context g_allocator;

void create_cabor_allocator_context(cabor_allocator_context* alloc_ctx)
{
    alloc_ctx->allocated_mem = 0;
}

void destroy_cabor_allocator_context(cabor_allocator_context* alloc_ctx)
{
    alloc_ctx->allocated_mem = 0;
}

void* cabor_malloc(cabor_allocator_context* alloc_ctx, size_t size)
{
    alloc_ctx->allocated_mem += size;
    return malloc(size);
}

void* cabor_realloc(cabor_allocator_context* alloc_ctx, void* mem, size_t size)
{
    alloc_ctx->allocated_mem = size;
    return realloc(mem, size);
}

void* cabor_calloc(cabor_allocator_context* alloc_ctx, size_t num, size_t size)
{
    alloc_ctx->allocated_mem += num * size;
    return calloc(num, size);
}

void cabor_free(cabor_allocator_context* alloc_ctx, void* mem)
{
    alloc_ctx->allocated_mem = 0;
    free(mem);
}

cabor_allocator_context* get_global_cabor_allocator_context()
{
    return &g_allocator;
}
