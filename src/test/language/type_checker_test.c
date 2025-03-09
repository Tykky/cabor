#include "type_checker_test.h"

#ifdef CABOR_ENABLE_TESTING

#include "../../language/type_checker.h"

static int test_typecheck_common(const char* code, size_t node_count, const char** expected)
{
    int res = 0;
    cabor_file* file = cabor_file_from_buffer(code, strlen(code));
    cabor_vector* tokens = cabor_tokenize(file);
    cabor_destroy_file(file);

    cabor_ast* ast = cabor_parse(tokens);
    cabor_vector* nodes = cabor_get_ast_node_list_al(ast->root);

    cabor_symbol_table* root_sym_table = cabor_create_symbol_table();
    cabor_ast_node* root = cabor_access_ast_node(ast->root);
    cabor_type type = cabor_typecheck(ast, root, root_sym_table);
    CABOR_CHECK_EQUALS(nodes->size, node_count, res);
    for (size_t i = 0; i < node_count; i++)
    {
        char buffer[256] = { 0 };
        cabor_ast_node_to_string(tokens, cabor_vector_get_ptr(nodes, i), buffer, 128, true);
        int comp = strcmp(buffer, expected[i]);
        //CABOR_LOG(buffer);
        if (comp != 0)
        {
            CABOR_LOG_ERR_F("EXPECTED: %s", expected[i]);
            CABOR_LOG_ERR_F("RECEIVED: %s", buffer);
        }
        CABOR_CHECK_EQUALS(comp, 0, res);
    }

    cabor_destroy_symbol_table(root_sym_table);
    cabor_destroy_vector(nodes);
    cabor_destroy_ast(ast);
    cabor_destroy_vector(tokens);

    return res;
}

int cabor_integration_test_typecheck_var_declaration()
{
    const char* code = "var x: Int = 1 + 1";
    const char* expected[] =
    {
        "root: var, edges: ['x', '+', 'Int'], type: 'Int'",
        "root: Int, edges: [], type: 'Unit'",
        "root: +, edges: ['1', '1'], type: 'Int'",
        "root: 1, edges: [], type: 'Int'",
        "root: 1, edges: [], type: 'Int'",
        "root: x, edges: [], type: 'Int'",
    };
    return test_typecheck_common(code, 6, expected);
}


#endif
