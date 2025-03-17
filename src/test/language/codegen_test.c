#include "codegen_test.h"
#include <string.h>

static void free_codegen_common(cabor_ir_data* ir_data, cabor_symbol_table* symbtab)
{
    cabor_destroy_ir_data(ir_data);
    cabor_destroy_symbol_table(symbtab);
}

static void codegen_common(cabor_ir_data** ir_data, cabor_symbol_table** symtab, const char* code)
{
     cabor_file* file = cabor_file_from_buffer(code, strlen(code));
     cabor_vector* tokens = cabor_tokenize(file);
     cabor_ast* ast = cabor_parse(tokens);
     cabor_ast_node* root = cabor_access_ast_node(ast->root);
     *symtab = cabor_create_symbol_table();
     cabor_type type = cabor_typecheck(ast, root, *symtab);
     *ir_data = cabor_create_ir_data();
     cabor_generate_ir(*ir_data, ast);

     cabor_locals* locals = cabor_create_locals();
     cabor_init_locals(*ir_data, locals);

     cabor_x64_assembly* asmbl = cabor_create_assembly();

     cabor_generate_assembly(*ir_data, locals, asmbl);

    cabor_vector* instructions = (*ir_data)->ir_instructions;

    for (size_t i = 0; i < instructions->size; i++)
    {
        cabor_ir_instruction* inst = cabor_vector_get_ir_instruction(instructions, i);
        char buffer[128] = {0};
        cabor_format_ir_instruction(*ir_data, i, buffer, 128);
        CABOR_LOG_F("IR %s", buffer);
    }

    for (size_t i = 0; i < asmbl->instructions->size; i++)
    {
        cabor_x64_instruction* inst = cabor_vector_get_x64_instruction(asmbl->instructions, i);
        CABOR_LOG_F("ASM %s", inst->text);
    }

     cabor_destroy_ast(ast);
     cabor_destroy_vector(tokens);
     cabor_destroy_file(file);
     cabor_destroy_locals(locals);
     cabor_destroy_x64_assembly(asmbl);

    free_codegen_common(*ir_data, *symtab);
}


int cabor_integration_test_codegen_basic()
{
    const char* code = "{ var x = true; if x then 1 else 2; }";
    cabor_ir_data* ir_data;
    cabor_symbol_table* symtab;
    codegen_common(&ir_data, &symtab, code);
    return 0;
}

int cabor_integration_test_codegen_print_int()
{
    const char* code = "print_int(1)";
    cabor_ir_data* ir_data;
    cabor_symbol_table* symtab;
    codegen_common(&ir_data, &symtab, code);
    return 0;
}

int cabor_compiler_test1()
{
    return 0;
}

