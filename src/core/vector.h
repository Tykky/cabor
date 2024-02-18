#pragma once

#include <stddef.h>
#include <stdbool.h>

#define CABOR_VECTOR_MULTIPLICATION_FACTOR 2

typedef struct
{
    char*  elements;
    int    stride;
    size_t capacity;
    size_t size;
} cabor_vector;

// Allocates vector of initial_size (numer of elements) where each element has size of stride in bytes
cabor_vector create_vector(size_t initial_size, int stride, bool zero_initialize);

// Pushback new element to vector. Note that the new element
// should be of the same size as provided in stride when 
// allocate_vector is called.
void pushback_vector(cabor_vector* v, void* element);

// Resizes the vector to size if size is larger than
// the current capacity of the vector.
void reserve_vector(cabor_vector* v, size_t size);

// Gets element specific index from the vector and
// does bounds checking. Note that casting the void*
// to the actual type before dereferncing is left to user.
void* vector_get(cabor_vector* v, size_t idx);

// Frees vector
void destroy_vector(cabor_vector* v);

// Gets next free element from the vector.
// this doesn't increment the current size
void* peek_next(cabor_vector* v);

// Pushes null terminated string to the vector.
// For strings we assume stride = 1
void push_string_to_vector(cabor_vector* v, const char* str, bool push_null_character);
