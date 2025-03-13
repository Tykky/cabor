#include "ir_test.h"
#include "../../language/ir.h"
#include <string.h>

static void ir_common(cabor_ir_data** ir_data, cabor_symbol_table** symtab, const char* code)
{
     cabor_file* file = cabor_file_from_buffer(code, strlen(code));
     cabor_vector* tokens = cabor_tokenize(file);
     cabor_ast* ast = cabor_parse(tokens);
     cabor_ast_node* root = cabor_access_ast_node(ast->root);
     *symtab = cabor_create_symbol_table();
     cabor_type type = cabor_typecheck(ast, root, symtab);
     *ir_data = cabor_create_ir_data();
     cabor_generate_ir(*ir_data, ast, root);

     cabor_destroy_ast(ast);
     cabor_destroy_vector(tokens);
     cabor_destroy_file(file);
}

void free_ir_common(cabor_ir_data* ir_data, cabor_symbol_table* symbtab)
{
    cabor_destroy_ir_data(ir_data);
    cabor_destroy_symbol_table(symbtab);
}

int cabor_integration_test_ir_basic_expression()
{
    const char* code = "1 + 2 * 3";

    cabor_ir_data* ir_data;
    cabor_symbol_table* symtab;

    ir_common(&ir_data, &symtab, code);

    free_ir_common(ir_data, symtab);

    return 0;
}
