#include "codegen.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define IS_IR_VAR_VALID(ir_var) strlen(ir_var->name) != 0

size_t cabor_get_stack_location_size()
{
    return sizeof(cabor_stack_location);
}

size_t cabor_get_x64_instruction_size()
{
    return sizeof(cabor_x64_instruction);
}

cabor_locals* cabor_create_locals()
{
    CABOR_NEW(cabor_locals, locals);
    locals->stack_used = cabor_create_vector(1024, CABOR_STACK_LOCATION, false);
    locals->stack_used = 0;
}

void cabor_emit_line(cabor_x64_assembly* asm, const char* fmt, ...)
{
    va_list args;

    CABOR_NEW(cabor_x64_instruction, inst);

    va_start(args, fmt);
    vsnprintf(inst->text, sizeof(cabor_x64_instruction), fmt, args);
    va_end(args);

    cabor_vector_push_x64_instruction(asm->instructions, inst);
}

void cabor_destroy_locals(cabor_locals* locals)
{
    cabor_destroy_vector(locals);
    CABOR_DELETE(cabor_locals, locals);
}

void cabor_init_locals(cabor_ir_data* ir_data, cabor_locals* locals)
{
    cabor_vector* ir_vars = ir_data->ir_vars;
    cabor_vector_reserve(locals->locations, ir_data->ir_vars->size);
    locals->locations->size = ir_data->ir_vars->size;
    for (cabor_ir_var_idx idx = 0; idx < ir_vars->size; idx++)
    {
        cabor_ir_var* ir_var = cabor_vector_get_ir_var(ir_data->ir_vars, idx);
        if (IS_IR_VAR_VALID(ir_var))
        {
            cabor_stack_location src_loc;
            snprintf(src_loc.location, CABOR_STACK_LOCATION_MAX_STR_SIZE, "-%d(%%rbp)", (idx + 1) * 8);

            cabor_stack_location* dst_loc = cabor_vector_get_stack_location(locals->locations, idx);
            memcpy(dst_loc, &src_loc, sizeof(cabor_stack_location));

            locals->stack_used += 8;
        }
    }
}

const char* cabor_get_stack_slot(cabor_ir_var_idx ir_var, cabor_locals* locals)
{
}

cabor_x64_assembly* cabor_generate_assembly(cabor_ir_data* ir_data, cabor_locals* locals)
{
    CABOR_NEW(cabor_x64_assembly, asm);
    asm->instructions = cabor_create_vector(1024, CABOR_X64_INSTRUCTION, false);

    for (cabor_ir_inst_idx idx = 0; idx < ir_data->ir_instructions; idx++)
    {
        cabor_ir_instruction* inst = cabor_vector_get_ir_instruction(ir_data->ir_instructions, idx);
        switch (inst->type)
        {
        case CABOR_IR_INST_LOAD_BOOL:
        {
            const char* dest = cabor_get_stack_slot(inst->load_bool_const.dest, locals);
            cabor_emit_mov_imm(asm, inst->load_bool_const.value ? 1 : 0, dest);
            break;
        }

        case CABOR_IR_INST_LOAD_INT:
        {
            const char* dest = cabor_get_stack_slot(inst->load_int_const.dest, locals);
            cabor_emit_mov_imm(asm, inst->load_int_const.value, dest);
            break;
        }

        case CABOR_IR_INST_COPY:
        {
            const char* src = cabor_get_stack_slot(inst->copy.source, locals);
            const char* dest = cabor_get_stack_slot(inst->copy.dest, locals);
            cabor_emit_mov_reg(asm, src, "%rax");
            cabor_emit_mov_reg(asm, "%rax", dest);
            break;
        }

        case CABOR_IR_INST_CONDJUMP:
        {
            const char* cond = cabor_get_stack_slot(inst->cond_jump.cond, locals);
            cabor_emit_cmp_imm(asm, 0, cond);
            cabor_emit_jne(asm, inst->cond_jump.then_label);
            cabor_emit_jmp(asm, inst->cond_jump.else_label);
            break;
        }

        case CABOR_IR_INST_JUMP:
        {
            cabor_emit_jmp(asm, inst->jump.label);
            break;
        }

        case CABOR_IR_INST_LABEL:
        {
            cabor_emit_label(asm, inst->label.idx);
            break;
        }

        case CABOR_IR_INST_CALL:
        {
            // For now we only support a single argument in %rdi
            if (inst->call.num_args > 0)
            {
                const char* arg0 = cabor_get_stack_slot(inst->call.args[0], locals);
                cabor_emit_mov_reg(asm, arg0, "%rdi");
            }

            cabor_emit_call(asm, inst->call.fun);

            if (inst->call.dest >= 0) // optional return
            {
                const char* dest = cabor_get_stack_slot(inst->call.dest, locals);
                cabor_emit_mov_reg(asm, "%rax", dest);
            }
            break;
        }

        case CABOR_IR_INST_UNKNOWN:
        default:
            fprintf(stderr, "Unknown instruction type at index %d\n", idx);
            break;
        }
    }
}

void cabor_destroy_x64_assembly(cabor_x64_assembly* asm)
{
    cabor_destroy_vector(asm->instructions);
    CABOR_DELETE(cabor_x64_assembly, asm);
}


void cabor_emit_mov_imm(cabor_x64_assembly* asm, int64_t imm, const char* dest)
{
    cabor_emit_line(asm, "movq $%ld, %s", imm, dest);
}

void cabor_emit_mov_reg(cabor_x64_assembly* asm, const char* src, const char* dest)
{
    cabor_emit_line(asm, "movq %s, %s", src, dest);
}

void cabor_emit_cmp_imm(cabor_x64_assembly* asm, int64_t imm, const char* operand)
{
    cabor_emit_line(asm, "cmpq $%ld, %s", imm, operand);
}

void cabor_emit_jmp(cabor_x64_assembly* asm, const char* label)
{
    cabor_emit_line(asm, "jmp %s", label);
}

void cabor_emit_jne(cabor_x64_assembly* asm, const char* label)
{
    cabor_emit_line(asm, "jne %s", label);
}

void cabor_emit_label(cabor_x64_assembly* asm, const char* label)
{
    cabor_emit_line(asm, "%s:", label);
}

void cabor_emit_call(cabor_x64_assembly* asm, const char* label)
{
    cabor_emit_line(asm, "call %s", label);
}
