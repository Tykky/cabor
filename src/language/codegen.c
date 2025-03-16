#include "codegen.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "../logging/logging.h"
#include "../debug/cabor_debug.h"

#define IS_IR_VAR_VALID(ir_var) strlen(ir_var->name) != 0

size_t cabor_get_stack_location_size()
{
    return sizeof(cabor_stack_location);
}

size_t cabor_get_x64_instruction_size()
{
    return sizeof(cabor_x64_instruction);
}

size_t cabor_get_x64_intrinsic_size()
{
    return sizeof(cabor_intrinsic);
}

cabor_locals* cabor_create_locals()
{
    CABOR_NEW(cabor_locals, locals);
    locals->locations = cabor_create_vector(1024, CABOR_STACK_LOCATION, false);
    locals->stack_used = 0;
    return locals;
}

void add_intrinsic(cabor_x64_assembly* asmbl, const char* name, cabor_intr_func intrinsic)
{
    cabor_intrinsic intr;

    if (strlen(name) < CABOR_MAX_X64_INTRINSIC_LENGTH)
    {
        strcpy(intr.name, name);
    }
    else
    {
        CABOR_LOG_ERR_F("max x64 intrinsic length exceeded for %s", name);
    }

    intr.intrinsic = intrinsic;

    cabor_vector_push_x64_instruction(asmbl->intrinsics, &intr);
}

void cabor_intr_unary_minus(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl) 
{
    cabor_emit_line(asmbl, "movq %s, %s", arg->arg_refs[0], arg->result_register);
    cabor_emit_line(asmbl, "negq %s", arg->result_register);
}

void cabor_intr_unary_not(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl)
{
    cabor_emit_line(asmbl, "movq %s, %s", arg->arg_refs[0], arg->result_register);
    cabor_emit_line(asmbl, "xorq $1, %s", arg->result_register);
}

void cabor_intr_plus(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl) 
{
    if (strcmp(arg->result_register, arg->arg_refs[0]) != 0) 
    {
        cabor_emit_line(asmbl, "movq %s, %s", arg->arg_refs[0], arg->result_register);
    }
    cabor_emit_line(asmbl, "addq %s, %s", arg->arg_refs[1], arg->result_register);
}

void cabor_intr_minus(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl) 
{
    if (strcmp(arg->result_register, arg->arg_refs[0]) != 0) 
    {
        cabor_emit_line(asmbl, "movq %s, %s", arg->arg_refs[0], arg->result_register);
    }
    cabor_emit_line(asmbl, "subq %s, %s", arg->arg_refs[1], arg->result_register);
}

void cabor_intr_multiply(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl)
{
    if (strcmp(arg->result_register, arg->arg_refs[0]) != 0) 
    {
        cabor_emit_line(asmbl, "movq %s, %s", arg->arg_refs[0], arg->result_register);
    }
    cabor_emit_line(asmbl, "imulq %s, %s", arg->arg_refs[1], arg->result_register);
}

void cabor_intr_divide(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl) 
{
    cabor_emit_line(asmbl, "movq %s, %%rax", arg->arg_refs[0]);
    cabor_emit_line(asmbl, "cqto");
    cabor_emit_line(asmbl, "idivq %s", arg->arg_refs[1]);
    if (strcmp(arg->result_register, "%rax") != 0) 
    {
        cabor_emit_line(asmbl, "movq %%rax, %s", arg->result_register);
    }
}

void cabor_intr_remainder(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl) 
{
    cabor_emit_line(asmbl, "movq %s, %%rax", arg->arg_refs[0]);
    cabor_emit_line(asmbl, "cqto");
    cabor_emit_line(asmbl, "idivq %s", arg->arg_refs[1]);
    if (strcmp(arg->result_register, "%rdx") != 0) 
    {
        cabor_emit_line(asmbl, "movq %%rdx, %s", arg->result_register);
    }
}

void cabor_intr_comparison(cabor_intrinsic_args* arg, const char* setcc_insn, cabor_x64_assembly* asmbl)
{
    cabor_emit_line(asmbl, "xor %%rax, %%rax");  // Clear all bits of rax
    cabor_emit_line(asmbl, "movq %s, %%rdx", arg->arg_refs[0]);
    cabor_emit_line(asmbl, "cmpq %s, %%rdx", arg->arg_refs[1]);
    cabor_emit_line(asmbl, "%s %%al", setcc_insn);  // Set lowest byte of rax to comparison result
    if (strcmp(arg->result_register, "%rax") != 0) 
    {
        cabor_emit_line(asmbl, "movq %%rax, %s", arg->result_register);
    }
}

void cabor_intr_eq(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl)
{
    cabor_intr_comparison(arg, "sete", asmbl);
}

void cabor_intr_ne(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl)
{
    cabor_intr_comparison(arg, "setne", asmbl);
}

void cabor_intr_lt(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl)
{
    cabor_intr_comparison(arg, "setl", asmbl);
}

void cabor_intr_le(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl)
{
    cabor_intr_comparison(arg, "setle", asmbl);
}

void cabor_intr_gt(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl)
{
    cabor_intr_comparison(arg, "setg", asmbl);
}

void cabor_intr_ge(cabor_intrinsic_args* arg, cabor_x64_assembly* asmbl)
{
    cabor_intr_comparison(arg, "setge", asmbl);
}

void cabor_emit_line(cabor_x64_assembly* asmbl, const char* fmt, ...)
{
    va_list args;

    cabor_x64_instruction inst;

    va_start(args, fmt);
    vsnprintf(inst.text, CABOR_MAX_X64_INSTRUCTION_LENGTH, fmt, args);
    va_end(args);

    cabor_vector_push_x64_instruction(asmbl->instructions, &inst);
}

void cabor_destroy_locals(cabor_locals* locals)
{
    cabor_destroy_vector(locals->locations);
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
    cabor_stack_location* loc = cabor_vector_get_stack_location(locals->locations, ir_var);
    return loc->location;
}

bool cabor_is_binary_args(int num_args)
{
    if (num_args != 2)
    {
        CABOR_LOG_ERR_F("codegen error: expected 2 args for binary CALL but got %d", num_args);
        return false;
    }
    return true;
}

void cabor_call_args_to_intrinisc_args(cabor_ir_data* ir_data, cabor_ir_call* call, cabor_intrinsic_args* args, cabor_locals* locals)
{
    cabor_ir_var_idx call_dest = call->dest;
    const char* resulterf = cabor_get_stack_slot(call_dest, locals);

    args->num_args = call->num_args;

    CABOR_ASSERT(args->num_args != 1 || args->num_args != 2, "invalid number of args");

    if (strlen(resulterf) < CABOR_MAX_X64_INTRINSIC_LENGTH)
    {
        strcpy(args->result_register, resulterf);
    }
    else
    {
        CABOR_LOG_ERR_F("codegen error: call result var was too small for intrinsic dest %s", resulterf);
    }

    for (size_t i = 0; i < call->num_args; i++)
    {
        cabor_ir_var_idx call_arg = call->args[i];
        cabor_ir_var* call_var = cabor_vector_get_ir_var(ir_data->ir_vars, call_arg);
        char* intr_arg = args->arg_refs[i];
        const char* callref = cabor_get_stack_slot(call_arg, locals);
        if (strlen(callref) < CABOR_MAX_X64_INTRINSIC_LENGTH)
        {
            strcpy(intr_arg, callref);
        }
        else
        {
            CABOR_LOG_ERR_F("codegen error: call var was too small for intrinsic arg %s", call_var->name);
        }
    }
}

void cabor_generate_assembly(cabor_ir_data* ir_data, cabor_locals* locals, cabor_x64_assembly* asmbl)
{
    for (cabor_ir_inst_idx idx = 0; idx < ir_data->ir_instructions->size; idx++)
    {
        cabor_ir_instruction* inst = cabor_vector_get_ir_instruction(ir_data->ir_instructions, idx);
        switch (inst->type)
        {
        case CABOR_IR_INST_LOAD_BOOL:
        {
            const char* dest = cabor_get_stack_slot(inst->load_bool_const.dest, locals);
            cabor_emit_mov_imm(asmbl, inst->load_bool_const.value ? 1 : 0, dest);
            break;
        }

        case CABOR_IR_INST_LOAD_INT:
        {
            const char* dest = cabor_get_stack_slot(inst->load_int_const.dest, locals);
            cabor_emit_mov_imm(asmbl, inst->load_int_const.value, dest);
            break;
        }

        case CABOR_IR_INST_COPY:
        {
            const char* src = cabor_get_stack_slot(inst->copy.source, locals);
            const char* dest = cabor_get_stack_slot(inst->copy.dest, locals);
            cabor_emit_mov_reg(asmbl, src, "%rax");
            cabor_emit_mov_reg(asmbl, "%rax", dest);
            break;
        }

        case CABOR_IR_INST_CONDJUMP:
        {
            const char* cond = cabor_get_stack_slot(inst->cond_jump.cond, locals);
            cabor_emit_cmp_imm(asmbl, 0, cond);
            cabor_ir_label* then_label = cabor_vector_get_ir_label(ir_data->ir_labels, inst->cond_jump.then_label);
            cabor_ir_label* else_label = cabor_vector_get_ir_label(ir_data->ir_labels, inst->cond_jump.else_label);
            cabor_emit_jne(asmbl, then_label);
            cabor_emit_jmp(asmbl, else_label);
            break;
        }

        case CABOR_IR_INST_JUMP:
        {
            cabor_ir_label* jmp_label = cabor_vector_get_ir_label(ir_data->ir_labels, inst->jump.label);
            cabor_emit_jmp(asmbl, jmp_label);
            break;
        }

        case CABOR_IR_INST_LABEL:
        {
            cabor_ir_label* inst_label = cabor_vector_get_ir_label(ir_data->ir_labels, inst->label.idx);
            cabor_emit_label(asmbl, inst_label->name);
            break;
        }

        case CABOR_IR_INST_CALL:
        {
            cabor_ir_call* call = &inst->call;
            cabor_ir_var* fun = cabor_vector_get_ir_var(ir_data->ir_vars, call->fun);
            cabor_intrinsic_args args;
            cabor_call_args_to_intrinisc_args(ir_data, call, &args, locals);

            if (strcmp(fun, "unary_-") == 0)
            {
                cabor_intr_unary_minus(&args, asmbl);
            }
            else if (strcmp(fun, "unary_not") == 0)
            {
                cabor_intr_unary_not(&args, asmbl);
            }
            else if (strcmp(fun, "+") == 0)
            {
                cabor_intr_plus(&args, asmbl);
            }
            else if (strcmp(fun, "-") == 0)
            {
                cabor_intr_minus(&args, asmbl);
            }
            else if (strcmp(fun, "*") == 0)
            {
                cabor_intr_multiply(&args, asmbl);
            }
            else if (strcmp(fun, "/") == 0)
            {
                cabor_intr_divide(&args, asmbl);
            }
            else if (strcmp(fun, "%") == 0)
            {
                cabor_intr_remainder(&args, asmbl);
            }
            else if (strcmp(fun, "==") == 0)
            {
                cabor_intr_eq(&args, asmbl);
            }
            else if (strcmp(fun, "!=") == 0)
            {
                cabor_intr_ne(&args, asmbl);
            }
            else if (strcmp(fun, "<") == 0)
            {
                cabor_intr_le(&args, asmbl);
            }
            else if (strcmp(fun, "<=") == 0)
            {
                cabor_intr_lt(&args, asmbl);
            }
            else if (strcmp(fun, ">") == 0)
            {
                cabor_intr_gt(&args, asmbl);
            }
            else if (strcmp(fun, ">=") == 0)
            {
                cabor_intr_ge(&args, asmbl);
            }

            // Handle non intrinsic calls

            const char* arg_regs[] = { "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9" };

            if (call->num_args > 6)
            {
                CABOR_LOG_ERR("codegen error: function call has more than 6 arguments, not supported");
                break;
            }

            for (size_t i = 0; i < call->num_args; i++)
            {
                const char* arg_slot = cabor_get_stack_slot(call->args[i], locals);
                cabor_emit_mov_reg(asmbl, arg_slot, arg_regs[i]);
            }

            cabor_emit_call(asmbl, fun->name);

            const char* dest = cabor_get_stack_slot(call->dest, locals);
            if (strcmp(dest, "%rax") != 0)
            {
                cabor_emit_mov_reg(asmbl, "%rax", dest);
            }

            break;
        }

        case CABOR_IR_INST_UNKNOWN:
        default:
            CABOR_LOG_ERR_F("Unknown instruction type at index %d\n", idx);
            break;
        }
    }
    return asmbl;
}

cabor_x64_assembly* cabor_create_assembly()
{
    CABOR_NEW(cabor_x64_assembly, asmbl);
    asmbl->instructions = cabor_create_vector(1024, CABOR_X64_INSTRUCTION, false);
    asmbl->intrinsics = cabor_create_vector(1024, CABOR_X64_INTRINSIC, false);
    return asmbl;
}

void cabor_destroy_x64_assembly(cabor_x64_assembly* asmbl)
{
    cabor_destroy_vector(asmbl->instructions);
    cabor_destroy_vector(asmbl->intrinsics);
    CABOR_DELETE(cabor_x64_assembly, asmbl);
}


void cabor_emit_mov_imm(cabor_x64_assembly* asmbl, int64_t imm, const char* dest)
{
    cabor_emit_line(asmbl, "movq $%ld, %s", imm, dest);
}

void cabor_emit_mov_reg(cabor_x64_assembly* asmbl, const char* src, const char* dest)
{
    cabor_emit_line(asmbl, "movq %s, %s", src, dest);
}

void cabor_emit_cmp_imm(cabor_x64_assembly* asmbl, int64_t imm, const char* operand)
{
    cabor_emit_line(asmbl, "cmpq $%ld, %s", imm, operand);
}

void cabor_emit_jmp(cabor_x64_assembly* asmbl, const char* label)
{
    cabor_emit_line(asmbl, "jmp %s", label);
}

void cabor_emit_jne(cabor_x64_assembly* asmbl, const char* label)
{
    cabor_emit_line(asmbl, "jne %s", label);
}

void cabor_emit_label(cabor_x64_assembly* asmbl, const char* label)
{
    cabor_emit_line(asmbl, "%s:", label);
}

void cabor_emit_call(cabor_x64_assembly* asmbl, const char* label)
{
    cabor_emit_line(asmbl, "call %s", label);
}
