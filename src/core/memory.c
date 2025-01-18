#include "memory.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../debug/cabor_debug.h"

static cabor_allocator_context g_allocator;

void create_cabor_allocator_context(cabor_allocator_context* alloc_ctx)
{
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
    alloc_ctx->allocated_mem = 0;
#endif
#if CABOR_ENABLE_MEMORY_DEBUGGING
    alloc_ctx->debug_size = 0;
#endif
}

void destroy_cabor_allocator_context(cabor_allocator_context* alloc_ctx)
{
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
    alloc_ctx->allocated_mem = 0;
#endif
#if CABOR_ENABLE_MEMORY_DEBUGGING
    alloc_ctx->debug_size = 0;
#endif
}

cabor_allocation cabor_malloc(cabor_allocator_context* alloc_ctx, size_t size, const char* debug)
{
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
    alloc_ctx->allocated_mem += size;
#endif
#if ENABLE_CABOR_MEMORY_DEBUGGING
    alloc_ctx->debug[alloc_ctx->debug_size++] = debug;
#endif

    cabor_allocation alloc =
    {
        .mem = malloc(size),
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
        .size = size
#endif
    };

    if (!alloc.mem)
    {
        fputs("Failed to cabor_malloc new memory!", stderr);
        assert(0);
        exit(1);
    }

    return alloc;
}

cabor_allocation cabor_realloc(cabor_allocator_context* alloc_ctx, cabor_allocation* old_alloc, size_t size, const char* debug)
{
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
    alloc_ctx->allocated_mem = alloc_ctx->allocated_mem - old_alloc->size + size;
#endif
#if CABOR_ENABLE_MEMORY_DEBUGGING
    alloc_ctx->debug[alloc_ctx->debug_size++] = debug;
#endif

    cabor_allocation new_alloc =
    {
        .mem = realloc(old_alloc->mem, size),
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
        .size = size
#endif
    };

    if (!new_alloc.mem)
    {
        fputs("Failed to cabor_realloc!", stderr);
        free(old_alloc->mem);
        assert(0);
        exit(1);
    }

    return new_alloc;
}

cabor_allocation cabor_calloc(cabor_allocator_context* alloc_ctx, size_t num, size_t size, const char* debug)
{
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
    alloc_ctx->allocated_mem += num * size;
#endif
#if CABOR_ENABLE_MEMORY_DEBUGGING
    alloc_ctx->debug[alloc_ctx->debug_size++] = debug;
#endif

    cabor_allocation alloc =
    {
        .mem = calloc(num, size),
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
        .size = num * size
#endif
    };

    if (!alloc.mem)
    {
        fputs("Failed to cabor_calloc", stderr);
        assert(0);
        exit(1);
    }

    return alloc;
}

void cabor_free(cabor_allocator_context* alloc_ctx, cabor_allocation* alloc, const char* dealloc)
{
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
    alloc_ctx->allocated_mem -= alloc->size;
#endif

#if CABOR_ENABLE_MEMORY_DEBUGGING
    alloc_ctx->dealloc[alloc_ctx->dealloc_size++] = dealloc;
#endif
    
    free(alloc->mem);

#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
    alloc->size = 0;
#endif
}

cabor_allocator_context* cabor_get_global_allocator_context()
{
    return &g_allocator;
}

size_t cabor_get_current_allocated(cabor_allocator_context* alloc_ctx)
{
    return alloc_ctx->allocated_mem;
}

const char* cabor_convert_bytes_to_human_readable(size_t bytes, double* converted)
{
    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    int suffixIndex = 0;

    *converted = (double)bytes;

    while (*converted >= 1024 && suffixIndex < 8) 
    {
        *converted /= 1024;
        suffixIndex++;
    }

    return suffixes[suffixIndex];
}
