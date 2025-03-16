#include "compiler.h"
#include "../logging/logging.h"

cabor_x64_assembly* cabor_compile(const char* code)
{
    cabor_ir_data* ir_data;
    cabor_symbol_table* symtab;

    cabor_file* file = cabor_file_from_buffer(code, strlen(code));
    cabor_vector* tokens = cabor_tokenize(file);
    cabor_ast* ast = cabor_parse(tokens);
    cabor_ast_node* root = cabor_access_ast_node(ast->root);
    symtab = cabor_create_symbol_table();
    cabor_type type = cabor_typecheck(ast, root, symtab);
    ir_data = cabor_create_ir_data();
    cabor_generate_ir(ir_data, ast);

    cabor_locals* locals = cabor_create_locals();
    cabor_init_locals(ir_data, locals);

    cabor_x64_assembly* asm = cabor_create_assembly();

    cabor_emit_line(asm, ".global _start\n");
    cabor_emit_line(asm, ".global main\n");
    cabor_emit_line(asm, ".extern print_int\n");
    cabor_emit_line(asm, ".section .text\n\n");

    cabor_emit_line(asm, "_start:\n");
    cabor_emit_line(asm, "    call main\n");
    cabor_emit_line(asm, "    movq $60, %%rax\n");
    cabor_emit_line(asm, "    xorq %%rdi, %%rdi\n");
    cabor_emit_line(asm, "    syscall\n\n");

    cabor_emit_line(asm, "main:\n");
    cabor_generate_assembly(ir_data, locals, asm);
    cabor_emit_line(asm, "    movq %%rax, -32(%%rbp)\n");
    cabor_emit_line(asm, "    ret\n\n");


    cabor_vector* instructions = (ir_data)->ir_instructions;

    for (size_t i = 0; i < instructions->size; i++)
    {
        cabor_ir_instruction* inst = cabor_vector_get_ir_instruction(instructions, i);
        char buffer[128] = {0};
        cabor_format_ir_instruction(ir_data, i, buffer, 128);
        CABOR_LOG_F("COMPILED IR: %s", buffer);
    }

    for (size_t i = 0; i < asm->instructions->size; i++)
    {
        cabor_x64_instruction* inst = cabor_vector_get_x64_instruction(asm->instructions, i);
        CABOR_LOG_F("COMPILED x64: %s", inst->text);
    }

    cabor_destroy_ast(ast);
    cabor_destroy_vector(tokens);
    cabor_destroy_file(file);
    cabor_destroy_locals(locals);

    cabor_destroy_ir_data(ir_data);
    cabor_destroy_symbol_table(symtab);

    return asm;
}
