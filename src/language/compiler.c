#include "compiler.h"
#include "../logging/logging.h"
#include <string.h>
#include <stdio.h>

cabor_x64_assembly* cabor_compile(const char* code, const char* filename)
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

    cabor_x64_assembly* asmbl = cabor_create_assembly();

    cabor_emit_line(asmbl, ".global _start\n");
    cabor_emit_line(asmbl, ".global main\n");
    cabor_emit_line(asmbl, ".extern print_int\n");
    cabor_emit_line(asmbl, ".section .text\n\n");

    cabor_emit_line(asmbl, "_start:\n");
    cabor_emit_line(asmbl, "    call main\n");
    cabor_emit_line(asmbl, "    movq $60, %%rax\n");
    cabor_emit_line(asmbl, "    xorq %%rdi, %%rdi\n");
    cabor_emit_line(asmbl, "    syscall\n\n");

    cabor_emit_line(asmbl, "main:\n");
    cabor_generate_assembly(ir_data, locals, asmbl);
    cabor_emit_line(asmbl, "    movq %%rax, -32(%%rbp)\n");
    cabor_emit_line(asmbl, "    ret\n\n");

    cabor_vector* instructions = (ir_data)->ir_instructions;

    for (size_t i = 0; i < instructions->size; i++)
    {
        cabor_ir_instruction* inst = cabor_vector_get_ir_instruction(instructions, i);
        char buffer[128] = {0};
        cabor_format_ir_instruction(ir_data, i, buffer, 128);
        CABOR_LOG_F("COMPILED IR: %s", buffer);
    }

    for (size_t i = 0; i < asmbl->instructions->size; i++)
    {
        cabor_x64_instruction* inst = cabor_vector_get_x64_instruction(asmbl->instructions, i);
        CABOR_LOG_F("COMPILED x64: %s", inst->text);
    }

    // write to output to a file and feed it to assembler + linker
    cabor_write_asmbl_to_file(filename, asmbl); // writes to "filename.asmbl"

    cabor_destroy_ast(ast);
    cabor_destroy_vector(tokens);
    cabor_destroy_file(file);
    cabor_destroy_locals(locals);

    cabor_destroy_ir_data(ir_data);
    cabor_destroy_symbol_table(symtab);

    return asmbl;
}

void cabor_write_asmbl_to_file(const char* filename, cabor_x64_assembly* asmbl)
{
    size_t total_size = 0;
    for (size_t i = 0; i < asmbl->instructions->size; i++)
    {
        cabor_x64_instruction* instr = cabor_vector_get_x64_instruction(asmbl->instructions, i);
        total_size += strlen(instr->text);
    }

    cabor_allocation alloc = CABOR_MALLOC(total_size);
    char* buffer = (char*)alloc.mem;
    char* line_begin = buffer;

    for (size_t i = 0; i < asmbl->instructions->size; i++)
    {
        cabor_x64_instruction* instr = cabor_vector_get_x64_instruction(asmbl->instructions, i);
        size_t line_size = strlen(instr->text);
        memcpy(line_begin, instr->text, line_size);
        line_begin += line_size;
    }


    cabor_file* file = cabor_file_from_buffer(buffer, total_size);

    char name[128] = {0};
    int result = snprintf(name, sizeof(name), "%s.asmbl", filename);

    if (result < sizeof(name))
    {
        cabor_dump_file_to_disk(file, name);
    }
    else
    {
        CABOR_LOG_ERR("Failed to write asmbl to file due filename was too large");
    }

    CABOR_FREE(&alloc);
    cabor_destroy_file(file);
}
