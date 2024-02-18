#pragma once

// TODO: implement proper allocator

#define CABOR_MALLOC(size) cabor_malloc(get_global_cabor_allocator_context(), size)
#define CABOR_REALLOC(mem, size) cabor_realloc(get_global_cabor_allocator_context(), mem, size)
#define CABOR_CALLOC(mem, size) cabor_calloc(get_global_cabor_allocator_context(), mem, size)
#define CABOR_FREE(mem) cabor_free(get_global_cabor_allocator_context(), mem)

#define CABOR_CREATE_ALLOCATOR() create_cabor_allocator_context(get_global_cabor_allocator_context())
#define CABOR_DESTROY_ALLOCATOR() destroy_cabor_allocator_context(get_global_cabor_allocator_context())

typedef struct
{
    void* mem;
    size_t size;
} allocation;

typedef struct 
{
    size_t allocated_mem;
} cabor_allocator_context;

void create_cabor_allocator_context(cabor_allocator_context* alloc_ctx);
void destroy_cabor_allocator_context(cabor_allocator_context* alloc_ctx);

void* cabor_malloc(cabor_allocator_context* alloc_ctx, size_t size);
void* cabor_realloc(cabor_allocator_context* alloc_ctx, void* mem, size_t size);
void* cabor_calloc(cabor_allocator_context* alloc_ctx, size_t num, size_t size);
void cabor_free(cabor_allocator_context* alloc_ctx, void* mem);

cabor_allocator_context* get_global_cabor_allocator_context();
