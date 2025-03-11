#include "vector.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "../logging/logging.h"
#include "../debug/cabor_debug.h"

size_t get_cabor_token_size();
size_t get_cabor_map_entry_size();

static size_t get_element_type_size(cabor_element_type type)
{
    switch (type)
    {
        case CABOR_FLOAT:
            return sizeof(float);
        case CABOR_DOUBLE:
            return sizeof(double);
        case CABOR_INT:
            return sizeof(int);
        case CABOR_UINT:
            return sizeof(unsigned int);
        case CABOR_CHAR:
            return sizeof(char);
        case CABOR_UCHAR:
            return sizeof(unsigned char);
        case CABOR_PTR:
            return sizeof(void*);
        case CABOR_TOKEN:
            return get_cabor_token_size();
        case CABOR_MAP_ENTRY:
            return get_cabor_map_entry_size();
        case CABOR_UNKNOWN:
            return 0;
    }
    return 0;
}

static void vector_resize(cabor_vector* v, size_t capacity)
{
    size_t stride = get_element_type_size(v->type);
    cabor_allocation alloc = CABOR_REALLOC(&v->vector_mem, stride * capacity);

    v->vector_mem = alloc;
    v->capacity = capacity;
}

static void pushback_vector(cabor_vector* v, void* element)
{
    size_t stride = get_element_type_size(v->type);

    if (v->size == v->capacity)
        vector_resize(v, v->capacity * CABOR_VECTOR_MULTIPLICATION_FACTOR);
    memcpy((char*) v->vector_mem.mem + stride * v->size++, element, stride);
}

static void* vector_get(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(idx < v->size, "Cabor vector index out of bounds!");
    size_t stride = get_element_type_size(v->type);
    return (char*) v->vector_mem.mem + stride * idx;
}

static void* peek_next(cabor_vector* v)
{
    size_t stride = get_element_type_size(v->type);
    return (char*) v->vector_mem.mem + stride * v->size;
}

cabor_vector* cabor_create_vector(size_t initial_capacity, cabor_element_type type, bool zero_initialize)
{
    size_t stride = get_element_type_size(type);
    CABOR_NEW(cabor_vector, v);

    *v = (cabor_vector)
    {
        .type       = type,
        .capacity   = initial_capacity,
        .size       = 0,
        .vector_mem = zero_initialize ? CABOR_CALLOC(initial_capacity, stride) : CABOR_MALLOC(initial_capacity * stride)
    };
    
    return v;
}

void cabor_vector_push_float(cabor_vector* v, float value)
{
    CABOR_ASSERT(v->type == CABOR_FLOAT, "pushing float to non float vector!");
    pushback_vector(v, (void*) & value);
}

void cabor_vector_push_double(cabor_vector* v, double value)
{
    CABOR_ASSERT(v->type == CABOR_DOUBLE, "pushing double to non double vector!");
    pushback_vector(v, (void*) & value);
}

void cabor_vector_push_int(cabor_vector* v, int value)
{
    CABOR_ASSERT(v->type == CABOR_INT, "pushing int to non int vector!");
    pushback_vector(v, (void*) & value);
}

void cabor_vector_push_uint(cabor_vector* v, unsigned int value)
{
    CABOR_ASSERT(v->type == CABOR_UINT, "pushing uint to non uint vector!");
    pushback_vector(v, (void*) & value);
}

void cabor_vector_push_char(cabor_vector* v, char value)
{
    CABOR_ASSERT(v->type == CABOR_CHAR, "pushing char to non char vector!");
    pushback_vector(v, (void*) & value);
}

void cabor_vector_push_uchar(cabor_vector* v, unsigned char value)
{
    CABOR_ASSERT(v->type == CABOR_UCHAR, "pushing uchar to non uchar vector!");
    pushback_vector(v, (void*) & value);
}

void cabor_vector_push_ptr(cabor_vector* v, void* ptr)
{
    CABOR_ASSERT(v->type == CABOR_PTR, "pushing ptr to non ptr vector!");
    pushback_vector(v, (void*) & ptr);
}

void cabor_vector_push_token(cabor_vector* v, struct cabor_token_t* token)
{
    CABOR_ASSERT(v->type == CABOR_TOKEN, "pushing token to non token vector!");
    pushback_vector(v, (void*) token);
}

void cabor_vector_push_map_entry(cabor_vector* v, struct cabor_map_entry_t* entry)
{
    CABOR_ASSERT(v->type == CABOR_MAP_ENTRY, "pushing map entry to non map entry vector!");
    pushback_vector(v, (void*) entry);
}

void cabor_vector_push_ir_instruction(cabor_vector* v, struct cabor_ir_instruction_t* instruction)
{
    CABOR_ASSERT(v->type == CABOR_IR_INSTRUCTION, "pushing ir instruction to non ir instruction vector!");
    pushback_vector(v, (void*)instruction);
}

void cabor_vector_push_ir_label(cabor_vector* v, struct cabor_ir_label_t* ir_label)
{
    CABOR_ASSERT(v->type == CABOR_IR_LABEL, "pushing ir label to non ir label vector!");
    pushback_vector(v, (void*)ir_label);
}

void cabor_vector_push_ir_var(cabor_vector* v, struct cabor_ir_var* ir_var)
{
    CABOR_ASSERT(v->type == CABOR_IR_VAR, "pushing ir var to non ir var vector!");
    pushback_vector(v, (void*)ir_var);
}

float cabor_vector_get_float(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_FLOAT, "getting float from non float vector!");
    return *(float*) vector_get(v, idx);
}

double cabor_vector_get_double(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_DOUBLE, "getting double from non double vector!");
    return *(double*) vector_get(v, idx);
}

int cabor_vector_get_int(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_INT, "getting int from non int vector!");
    return *(int*) vector_get(v, idx);
}

unsigned int cabor_vector_get_uint(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_UINT, "getting uint from non uint vector!");
    return *(unsigned int*) vector_get(v, idx);
}

char cabor_vector_get_char(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_CHAR, "getting char from non char vector!");
    return *(char*) vector_get(v, idx);
}

unsigned char cabor_vector_get_uchar(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_UCHAR, "getting uchar from non uchar vector!");
    return *(unsigned char*) vector_get(v, idx);
}

void* cabor_vector_get_ptr(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_PTR, "getting ptr from non ptr vector!");
    return *(void**)vector_get(v, idx);
}

struct cabor_token_t* cabor_vector_get_token(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_TOKEN, "getting token from non token vector!");
    return (struct cabor_token_t*)vector_get(v, idx);
}

struct cabor_map_entry_t* cabor_vector_get_map_entry(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_MAP_ENTRY, "getting map entry from non map entry vector!");
    return (struct cabor_map_entry_t*)vector_get(v, idx);
}

struct cabor_instruction_t* cabor_vector_get_ir_instruction(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_IR_INSTRUCTION, "getting ir instruction from non ir instruction vector!");
    return (struct cabor_ir_instruction_t*)vector_get(v, idx);
}

struct cabor_ir_var_t* cabor_vector_get_ir_var(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_IR_INSTRUCTION, "pushing ir var from non ir var vector!");
    return (struct cabor_ir_var_t*)vector_get(v, idx);
}

struct cabor_ir_label_t* cabor_vector_get_ir_label(cabor_vector* v, size_t idx)
{
    CABOR_ASSERT(v->type == CABOR_IR_LABEL, "pushing ir label from non ir label vector!");
    return (struct cabor_ir_label_t*)vector_get(v, idx);
}

void cabor_vector_push_str(cabor_vector* v, const char* str, bool push_null_character)
{
    size_t idx = 0;
    char null_character = '\0';

    while (str[idx] != '\0')
    {
        cabor_vector_push_char(v, str[idx++]);
    }

    if (push_null_character)
        cabor_vector_push_char(v, null_character);
}

void cabor_vector_reserve(cabor_vector* v, size_t size)
{
    if (v->capacity >= size)
        return;
    vector_resize(v, size);
}

void cabor_destroy_vector(cabor_vector* v)
{
    CABOR_FREE(&v->vector_mem);
    v->vector_mem.mem = NULL;
    v->type = 0;
    v->capacity = 0;
    v->size = 0;
    CABOR_DELETE(cabor_vector, v);
}

float* cabor_vector_peek_float(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_FLOAT, "Tried to peek_float non float vector!");
    return (float*)peek_next(v);
}

double* cabor_vector_peek_double(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_DOUBLE, "Tried to peek_double non double vector!");
    return (double*)peek_next(v);
}

int* cabor_vector_peek_int(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_INT, "Tried to peek_int non int vector!");
    return (int*)peek_next(v);
}

unsigned int* cabor_vector_peek_uint(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_UINT, "Tried to peek_uint non uint vector!");
    return (unsigned int*)peek_next(v);
}

char* cabor_vector_peek_char(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_CHAR, "Tried to peek_char non char vector!");
    return (char*)peek_next(v);
}

unsigned char* cabor_vector_peek_uchar(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_UCHAR, "Tried to peek_uchar non uchar vector!");
    return (unsigned char*)peek_next(v);
}

void** cabor_vector_peek_ptr(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_PTR, "Tried to peek_ptr non ptr vector!");
    return (void**)peek_next(v);
}

struct cabor_token_t* cabor_vector_peek_token(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_TOKEN, "Tried to peek_token non token vector!");
    return (struct cabor_token_t*)peek_next(v);
}

struct cabor_map_entry_t* cabor_vector_peek_map_entry(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_MAP_ENTRY, "Tried to peek_map_entry non token vector!");
    return (struct cabor_map_entry_t*)peek_next(v);
}

struct cabor_instruction_t* cabor_peek_ir_instruction(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_IR_INSTRUCTION, "getting ir instruction from non ir instruction vector!");
    return (struct cabor_ir_instruction_t*)peek_next(v);
}

struct cabor_ir_var_t* cabor_peek_ir_var(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_IR_VAR, "getting ir var from non ir var vector!");
    return (struct cabor_ir_var_t*)peek_next(v);
}

struct cabor_ir_label_t* cabor_peek_ir_label(cabor_vector* v)
{
    CABOR_ASSERT(v->type == CABOR_IR_LABEL, "getting ir label from non ir label vector!");
    return (struct cabor_ir_lavel*)peek_next(v);
}

