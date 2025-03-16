#pragma once

#include "ir.h"
#include "../core/hashmap.h"

#define CABOR_STACK_LOCATION_MAX_STR_SIZE 64
#define CABOR_MAX_X64_INSTRUCTION_LENGTH 64

size_t cabor_get_stack_location_size();
size_t cabor_get_x64_instruction_size();

typedef struct cabor_stack_location_t
{
    char* location[CABOR_STACK_LOCATION_MAX_STR_SIZE];
} cabor_stack_location;

typedef struct cabor_x64_instruction_t
{
    char* text[CABOR_MAX_X64_INSTRUCTION_LENGTH];
} cabor_x64_instruction;

typedef struct
{
    cabor_vector* locations; // maps ir_var (int) -> stack location
    size_t stack_used;
} cabor_locals;

typedef struct
{
    cabor_vector* instructions;
} cabor_x64_assembly;

void cabor_emit_line(cabor_x64_assembly* asm, const char* fmt, ...);

cabor_locals* cabor_create_locals();
void cabor_destroy_locals(cabor_locals* locals);

const char* cabor_get_stack_slot(cabor_ir_var_idx ir_var, cabor_locals* locals);

void cabor_init_locals(cabor_ir_data* ir_data, cabor_locals* cabor_locals);
cabor_x64_assembly* cabor_generate_assembly(cabor_ir_data* ir_data, cabor_locals* locals);
void cabor_destroy_x64_assembly(cabor_x64_assembly* asm);

void cabor_emit_mov_imm(cabor_x64_assembly* asm, int64_t imm, const char* dest);
void cabor_emit_mov_reg(cabor_x64_assembly* asm, const char* src, const char* dest);
void cabor_emit_cmp_imm(cabor_x64_assembly* asm, int64_t imm, const char* operand);
void cabor_emit_jmp(cabor_x64_assembly* asm, const char* label);
void cabor_emit_jne(cabor_x64_assembly* asm, const char* label);
void cabor_emit_label(cabor_x64_assembly* asm, const char* label);
void cabor_emit_call(cabor_x64_assembly* asm, const char* label);
