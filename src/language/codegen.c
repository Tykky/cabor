#include "codegen.h"
#include <string.h>
#include <stdio.h>

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

cabor_x64_assembly* cabor_generate_assembly(cabor_ir_data* ir_data)
{
    CABOR_NEW(cabor_x64_assembly, asm);
    asm->instructions = cabor_create_vector(1024, CABOR_X64_INSTRUCTION, false);
}

void cabor_destroy_x64_assembly(cabor_x64_assembly* asm)
{
    cabor_destroy_vector(asm->instructions);
    CABOR_DELETE(cabor_x64_assembly, asm);
}
