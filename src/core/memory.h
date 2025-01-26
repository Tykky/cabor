#pragma once

#include "../cabor_defines.h"

#include <stddef.h>

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define LINE_STRING STRINGIZE(__LINE__)
#define FUNC_STRING __func__

#define CABOR_MEMORY_DEBUG_STR_ALLOC __FILE__ ":" LINE_STRING " ALLOCATION\n"
#define CABOR_MEMORY_DEBUG_STR_DEALLOC __FILE__ ":" LINE_STRING " DEALLOCATION\n"

#define CABOR_MALLOC(size) cabor_malloc(cabor_get_global_allocator_context(), size, CABOR_MEMORY_DEBUG_STR_ALLOC)
#define CABOR_REALLOC(mem, size) cabor_realloc(cabor_get_global_allocator_context(), mem, size, CABOR_MEMORY_DEBUG_STR_ALLOC)
#define CABOR_CALLOC(num, size) cabor_calloc(cabor_get_global_allocator_context(), num, size, CABOR_MEMORY_DEBUG_STR_ALLOC)
#define CABOR_FREE(mem) cabor_free(cabor_get_global_allocator_context(), mem, CABOR_MEMORY_DEBUG_STR_DEALLOC)

#define CABOR_CREATE_ALLOCATOR() create_cabor_allocator_context(cabor_get_global_allocator_context())
#define CABOR_DESTROY_ALLOCATOR() destroy_cabor_allocator_context(cabor_get_global_allocator_context())

#define CABOR_GET_ALLOCATED() cabor_get_current_allocated(cabor_get_global_allocator_context())
#define CABOR_GET_ALLOCATOR() cabor_get_global_allocator_context()

#define CABOR_MEMORY_DEBUG_ARRAY_SIZE 4096

#ifdef NDEBUG
#define CABOR_ENABLE_MEMORY_DEBUGGING 0
#else
#define CABOR_ENABLE_MEMORY_DEBUGGING 1
#endif

#if CABOR_ENABLE_MEMORY_DEBUGGING
#include "../core/mutex.h"
#define CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
#endif

// quality of life macro to generate cabor_allocation struct when assigning value to variable
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
#define CABOR_DEALLOC(type, memory) { .mem = memory, .size = sizeof(type) }
#else
#define CABOR_DEALLOC(type, memory) { .mem = memory }
#endif

// These are not the nicest macros to use but behave similarly to C++ new and delete.
// Would be nice to have decltype() so we could avoid passing the type
#define CABOR_NEW(type, variable)\
cabor_allocation variable##_alloc = CABOR_MALLOC(sizeof(type));\
type* variable = variable##_alloc.mem

#define CABOR_DELETE(type, variable)\
cabor_allocation variable##_alloc = CABOR_DEALLOC(type, variable);\
CABOR_FREE(&variable##_alloc)

typedef struct
{
    void* mem;
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
    size_t size;
#endif
} cabor_allocation;

typedef struct 
{
    size_t allocated_mem;

#if CABOR_ENABLE_MEMORY_DEBUGGING
    // Debug info for allocations
    char* debug[CABOR_MEMORY_DEBUG_ARRAY_SIZE];
    size_t debug_size;
    cabor_mutex* alloc_lock;

    // Debug info for deallocation
    char* dealloc[CABOR_MEMORY_DEBUG_ARRAY_SIZE];
    size_t dealloc_size;
    cabor_mutex* dealloc_lock;

#endif

} cabor_allocator_context;

void create_cabor_allocator_context  (cabor_allocator_context* alloc_ctx);
void destroy_cabor_allocator_context (cabor_allocator_context* alloc_ctx);

cabor_allocation cabor_malloc  (cabor_allocator_context* alloc_ctx, size_t size, const char* debug);
cabor_allocation cabor_realloc (cabor_allocator_context* alloc_ctx, cabor_allocation* old_alloc, size_t size, const char* debug);
cabor_allocation cabor_calloc  (cabor_allocator_context* alloc_ctx, size_t num, size_t size, const char* debug);
void             cabor_free    (cabor_allocator_context* alloc_ctx, cabor_allocation* alloc, const char* dealloc);

cabor_allocator_context* cabor_get_global_allocator_context();

size_t cabor_get_current_allocated(cabor_allocator_context* alloc_ctx);
const char* cabor_convert_bytes_to_human_readable(size_t bytes, double* converted);
