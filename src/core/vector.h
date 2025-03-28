#pragma once

#include "../cabor_defines.h"
#include "../core/memory.h"

#include <stddef.h>
#include <stdbool.h>

#define CABOR_VECTOR_MULTIPLICATION_FACTOR 2

struct cabor_token_t;
struct cabor_map_entry_t;
struct cabor_ir_instruction_t;
struct cabor_ir_var_t;
struct cabor_ir_label_t;
struct cabor_stack_location_t;
struct cabor_x64_instruction_t;
struct cabor_intrinsic_t;

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
    CABOR_MAP_ENTRY,
    CABOR_IR_INSTRUCTION,
    CABOR_IR_VAR,
    CABOR_IR_LABEL,
    CABOR_STACK_LOCATION,
    CABOR_X64_INSTRUCTION,
    CABOR_X64_INTRINSIC,
    CABOR_UNKNOWN
} cabor_element_type;

typedef struct
{
    cabor_element_type type;
    size_t             capacity;
    size_t             size;
    cabor_allocation   vector_mem;
} cabor_vector;


cabor_vector* cabor_create_vector(size_t initial_capacity, cabor_element_type type, bool zero_initialize);

void cabor_vector_push_float  (cabor_vector* v, float value);
void cabor_vector_push_double (cabor_vector* v, double value);
void cabor_vector_push_int    (cabor_vector* v, int value);
void cabor_vector_push_uint   (cabor_vector* v, unsigned int value);
void cabor_vector_push_char   (cabor_vector* v, char value);
void cabor_vector_push_uchar  (cabor_vector* v, unsigned char value);
void cabor_vector_push_ptr    (cabor_vector* v, void* ptr);
void cabor_vector_push_token  (cabor_vector* v, struct cabor_token_t* token);
void cabor_vector_push_map_entry (cabor_vector* v, struct cabor_map_entry_t* token);
void cabor_vector_push_ir_instruction (cabor_vector* v, struct cabor_ir_instruction_t* instruction);
void cabor_vector_push_ir_var (cabor_vector* v, struct cabor_ir_var_t* ir_var);
void cabor_vector_push_ir_label (cabor_vector* v, struct cabor_ir_label_t* ir_label);
void cabor_vector_push_stack_location (cabor_vector* v, struct cabor_stack_location_t* stack_location);
void cabor_vector_push_x64_instruction (cabor_vector* v, struct cabor_x64_instruction_t* instruction);
void cabor_vector_push_x64_intrinsic (cabor_vector* v, struct cabor_intrinsic_t* intrinsic);

void cabor_vector_push_str(cabor_vector* v, const char* str, bool push_null_character);

float         cabor_vector_get_float  (cabor_vector* v, size_t idx);
double        cabor_vector_get_double (cabor_vector* v, size_t idx);
int           cabor_vector_get_int    (cabor_vector* v, size_t idx);
unsigned int  cabor_vector_get_uint   (cabor_vector* v, size_t idx);
char          cabor_vector_get_char   (cabor_vector* v, size_t idx);
unsigned char cabor_vector_get_uchar  (cabor_vector* v, size_t idx);
void*         cabor_vector_get_ptr    (cabor_vector* v, size_t idx);
struct cabor_token_t*  cabor_vector_get_token  (cabor_vector* v, size_t idx);
struct cabor_map_entry_t*  cabor_vector_get_map_entry  (cabor_vector* v, size_t idx);
struct cabor_instruction_t* cabor_vector_get_ir_instruction (cabor_vector* v, size_t idx);
struct cabor_ir_var_t* cabor_vector_get_ir_var (cabor_vector* v, size_t idx);
struct cabor_ir_label_t* cabor_vector_get_ir_label (cabor_vector* v, size_t idx);
struct cabor_stack_location_t* cabor_vector_get_stack_location (cabor_vector* v, size_t idx);
struct cabor_x64_instruction_t* cabor_vector_get_x64_instruction (cabor_vector* v, size_t idx);
struct cabor_intrinsic_t* cabor_vector_get_x64_intrinsic (cabor_vector* v, size_t idx);

void cabor_vector_reserve(cabor_vector* v, size_t size);

void cabor_destroy_vector(cabor_vector* v);

float*         cabor_vector_peek_float  (cabor_vector* v);
double*        cabor_vector_peek_double (cabor_vector* v);
int*           cabor_vector_peek_int    (cabor_vector* v);
unsigned int*  cabor_vector_peek_uint   (cabor_vector* v);
char*          cabor_vector_peek_char   (cabor_vector* v);
unsigned char* cabor_vector_peek_uchar  (cabor_vector* v);
void**         cabor_vector_peek_ptr    (cabor_vector* v);
struct cabor_token_t*   cabor_vector_peek_token  (cabor_vector* v);
struct cabor_map_entry_t* cabor_vector_peek_map_entry  (cabor_vector* v);
struct cabor_instruction_t* cabor_peek_ir_instruction (cabor_vector* v);
struct cabor_ir_label_t* cabor_peek_ir_label (cabor_vector* v);
struct cabor_stack_location_t* cabor_peek_stack_location (cabor_vector* v);
struct cabor_x64_instruction_t* cabor_peek_x64_instruction (cabor_vector* v);
struct cabor_intrinsic_t* cabor_peek_x64_intrinsic (cabor_vector* v);
