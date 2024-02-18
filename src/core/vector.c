#include "vector.h"
#include <string.h>
#include <assert.h>

#include "memory.h"

static void vector_resize(cabor_vector* v, size_t capacity)
{
    void* ptr = v->elements;
    size_t new_size = v->stride * capacity;

    char* elements = CABOR_REALLOC(v->elements, v->stride * capacity);
    if (!elements)
    {
        perror("Failed to realloc a new cabor vector!");
        exit(1);
    }

    v->elements = elements;
    v->capacity = capacity;
}

cabor_vector create_vector(size_t initial_size, int stride, bool zero_initialize)
{
    cabor_vector v =
    {
        .elements = zero_initialize ? CABOR_CALLOC(initial_size, stride) : CABOR_MALLOC(initial_size * stride),
        .stride   = stride,
        .capacity = initial_size,
        .size     = 0
    };
    
    if (!v.elements)
    {
        perror("Failed to malloc a new cabor vector!");
        exit(1);
    }

    return v;
}

void pushback_vector(cabor_vector* v, void* element)
{
    if (v->size == v->capacity)
        vector_resize(v, v->capacity * CABOR_VECTOR_MULTIPLICATION_FACTOR);
    memcpy(v->elements + v->stride * v->size++, element, v->stride);
}

void reserve_vector(cabor_vector* v, size_t size)
{
    if (v->capacity >= size)
        return;
    vector_resize(v, size);
}

void* vector_get(cabor_vector* v, size_t idx)
{
    if (idx >= v->size)
    {
        perror("Cabor vector index out of bounds!");
        exit(1);
    }
    return v->elements + v->stride * idx;
}

void destroy_vector(cabor_vector* v)
{
    free(v->elements);
    v->elements = NULL;
    v->stride = 0;
    v->capacity = 0;
    v->size = 0;
}

void* peek_next(cabor_vector* v)
{
    return v->elements + v->stride * v->size;
}

void push_string_to_vector(cabor_vector* v, const char* str, bool push_null_character)
{
    assert(v->stride = 1);

    size_t idx = 0;
    char null_character = '\0';

    while (str[idx] != '\0')
    {
        pushback_vector(v, &str[idx]);
        idx++;
    }

    if (push_null_character)
        pushback_vector(v, &null_character);
}
