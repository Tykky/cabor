#pragma once

#include "../cabor_defines.h"
#include "../core/memory.h"

#include <stddef.h>
#include <stdbool.h>

#define CABOR_VECTOR_MULTIPLICATION_FACTOR 2

typedef struct cabor_token cabor_token;

// Similar to std::vector from C++. Since C doesn't support function overloading or templates we 
// manually create 'overload' for each type. If Debug build is used the implementation 
// does runtime checks for types and out of bounds.

typedef enum
{
    CABOR_FLOAT,
    CABOR_DOUBLE,
    CABOR_INT,
    CABOR_UINT,
    CABOR_CHAR,
    CABOR_UCHAR,
    CABOR_PTR,
    CABOR_TOKEN,
    CABOR_UNKNOWN
} cabor_element_type;

typedef struct
{
    cabor_element_type type;
    size_t             capacity;
    size_t             size;
    cabor_allocation   vector_mem;
} cabor_vector;

cabor_vector create_cabor_vector(size_t initial_capacity, cabor_element_type type, bool zero_initialize);

void cabor_vector_push_float  (cabor_vector* v, float value);
void cabor_vector_push_double (cabor_vector* v, double value);
void cabor_vector_push_int    (cabor_vector* v, int value);
void cabor_vector_push_uint   (cabor_vector* v, unsigned int value);
void cabor_vector_push_char   (cabor_vector* v, char value);
void cabor_vector_push_uchar  (cabor_vector*v, unsigned char value);
void cabor_vector_push_ptr    (cabor_vector* v, void* ptr);
void cabor_vector_push_token  (cabor_vector* v, cabor_token* token);

void cabor_vector_push_str(cabor_vector* v, const char* str, bool push_null_character);

float         cabor_vector_get_float  (cabor_vector* v, size_t idx);
double        cabor_vector_get_double (cabor_vector* v, size_t idx);
int           cabor_vector_get_int    (cabor_vector* v, size_t idx);
unsigned int  cabor_vector_get_uint   (cabor_vector* v, size_t idx);
char          cabor_vector_get_char   (cabor_vector* v, size_t idx);
unsigned char cabor_vector_get_uchar  (cabor_vector* v, size_t idx);
void*         cabor_vector_get_ptr    (cabor_vector* v, size_t idx);
cabor_token*  cabor_vector_get_token  (cabor_vector* v, size_t idx);

void cabor_vector_reserve(cabor_vector* v, size_t size);

void destroy_cabor_vector(cabor_vector* v);

float*         cabor_vector_peek_float  (cabor_vector* v);
double*        cabor_vector_peek_double (cabor_vector* v);
int*           cabor_vector_peek_int    (cabor_vector* v);
unsigned int*  cabor_vector_peek_uint   (cabor_vector* v);
char*          cabor_vector_peek_char   (cabor_vector* v);
unsigned char* cabor_vector_peek_uchar  (cabor_vector* v);
void**         cabor_vector_peek_ptr    (cabor_vector* v);
cabor_token*   cabor_vector_peek_token  (cabor_vector* v, size_t idx);
