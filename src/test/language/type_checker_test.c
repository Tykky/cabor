#include "type_checker_test.h"

#ifdef CABOR_ENABLE_TESTING

#include <string.h>
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

static int test_typecheck_common_expect_fail(const char* code)
{
    int res = 0;
    cabor_file* file = cabor_file_from_buffer(code, strlen(code));
    cabor_vector* tokens = cabor_tokenize(file);
    cabor_destroy_file(file);

    cabor_ast* ast = cabor_parse(tokens);

    cabor_symbol_table* root_sym_table = cabor_create_symbol_table();
    cabor_ast_node* root = cabor_access_ast_node(ast->root);
    cabor_type type = cabor_typecheck(ast, root, root_sym_table);

    CABOR_CHECK_EQUALS(type, CABOR_TYPE_ERROR, res);

    cabor_destroy_symbol_table(root_sym_table);
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

int cabor_integration_test_typecheck_var_declaration2()
{
    const char* code = "{ var x: Int = 1 + 1; x = 3}";
    const char* expected[] =
    {
        "root: {, edges: ['var', '='], type: 'Int'",
        "root: =, edges: ['x', '3'], type: 'Int'",
        "root: 3, edges: [], type: 'Int'",
        "root: x, edges: [], type: 'Int'",
        "root: var, edges: ['x', '+', 'Int'], type: 'Int'",
        "root: Int, edges: [], type: 'Unit'",
        "root: +, edges: ['1', '1'], type: 'Int'",
        "root: 1, edges: [], type: 'Int'",
        "root: 1, edges: [], type: 'Int'",
        "root: x, edges: [], type: 'Int'",
    };
    return test_typecheck_common(code, 10, expected);
}

int cabor_integration_test_typecheck_if_then_else()
{
    const char* code = "if true then 1 else 2";
    const char* expected[] =
    {
        "root: if, edges: ['true', '1', '2'], type: 'Int'",
        "root: 2, edges: [], type: 'Int'",
        "root: 1, edges: [], type: 'Int'",
        "root: true, edges: [], type: 'Bool'",
    };
    return test_typecheck_common(code, 4, expected);
}

int cabor_integration_test_typecheck_unary_op_not()
{
    const char* code = "not true";
    const char* expected[] =
    {
        "root: not, edges: ['true'], type: 'Bool'",
        "root: true, edges: [], type: 'Bool'",
    };
    return test_typecheck_common(code, 2, expected);
}

int cabor_integration_test_typecheck_unary_op_minus()
{
    const char* code = "-1";
    const char* expected[] =
    {
        "root: -, edges: ['1'], type: 'Int'",
        "root: 1, edges: [], type: 'Int'",
    };
    return test_typecheck_common(code, 2, expected);
}

int cabor_integration_test_typecheck_while_loop()
{
    const char* code = "while true do 1";
    const char* expected[] =
    {
        "root: while, edges: ['true', '1'], type: 'Unit'",
        "root: 1, edges: [], type: 'Int'",
        "root: true, edges: [], type: 'Bool'",
    };
    return test_typecheck_common(code, 3, expected);
}

int cabor_integration_test_typecheck_undeclared_identifier()
{
    const char* code = "x + 1";
    return test_typecheck_common_expect_fail(code);
}

int cabor_integration_test_typecheck_mismatched_binary_op()
{
    const char* code = "1 + True";
    return test_typecheck_common_expect_fail(code);
}

int cabor_integration_test_typecheck_mismatched_variable_decl()
{
    const char* code = "var x: Bool = 123";
    return test_typecheck_common_expect_fail(code);
}

int cabor_integration_test_typecheck_duplicate_decl_same_scope()
{
    const char* code = "{ var x: Int = 1; var x: Int = 2 }";
    return test_typecheck_common_expect_fail(code);
}

int cabor_integration_test_typecheck_mismatched_branch_types()
{
    const char* code = "if True then 1 else False";
    return test_typecheck_common_expect_fail(code);
}

int cabor_integration_test_typecheck_not_bool_if()
{
    const char* code = "if 42 then 1 else 2";
    return test_typecheck_common_expect_fail(code);
}

int cabor_integration_test_typecheck_not_bool_while()
{
    const char* code = "while 3 do 1";
    return test_typecheck_common_expect_fail(code);
}

int cabor_integration_test_typecheck_scoping_rules()
{
    const char* code = "{ var x: Int = 1; { var y: Int = x + 1 } }";
    return test_typecheck_common_expect_fail(code);
}

#endif
