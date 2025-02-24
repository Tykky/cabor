#include "parser_test.h"

#include "../../core/vector.h"
#include "../../language/tokenizer.h"

#include <string.h>

#ifdef CABOR_ENABLE_TESTING

// Normally we produce tokens from a file but for testing we are creating
// them manually here
static cabor_token create_token(const char* data, cabor_token_type type)
{
    cabor_token t;
    t.type = type;
    size_t size = strlen(data);
    strcpy(t.data, data);
    return t;
}

// Unit tests, these are a bit verbose so we don't have many of them.
// The integration tests are way more compact and do the bulk of the work when it comes to testing

// Try to parse expression: a + b * c
int cabor_test_parse_expression_abc()
{
    cabor_vector* tokens = cabor_create_vector(100, CABOR_TOKEN, true);

    cabor_token tmp[5] = { 0 };
    size_t i = 0;

    tmp[i++] = create_token("a", CABOR_IDENTIFIER);
    tmp[i++] = create_token("+", CABOR_OPERATOR);
    tmp[i++] = create_token("b", CABOR_IDENTIFIER);
    tmp[i++] = create_token("*", CABOR_OPERATOR);
    tmp[i++] = create_token("c", CABOR_IDENTIFIER);

    for (size_t i = 0; i < 5; i++)
    {
        cabor_vector_push_token(tokens, &tmp[i]);
    }

    char token_string[100] = { 0 };

    cabor_stringify_tokens(token_string, 100, tokens);

    const char* expected_tokens = "['a', '+', 'b', '*', 'c']";

    int test = strcmp(expected_tokens, token_string);

    if (test)
    {
        CABOR_LOG_ERR_F("EXPECTED: %s", expected_tokens);
        CABOR_LOG_ERR_F("ACTUAL  : %s", token_string);
    }

    int res = 0;
    CABOR_CHECK_EQUALS(test, 0, res);

    size_t cursor = 0;
    cabor_ast_allocated_node ast = cabor_parse_expression(tokens, &cursor);
    cabor_vector* ast_nodes = cabor_get_ast_node_list_al(&ast);

    char buffer[100] = { 0 };

    for (size_t i = 0; i < ast_nodes->size; i++)
    {
        cabor_ast_node* n = cabor_vector_get_ptr(ast_nodes, i);
        cabor_ast_allocated_node an =
        {
            .node_mem.mem = n,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .node_mem.size = sizeof(cabor_ast_node),
#endif
        };
        memset(buffer, 0, 100);
        cabor_ast_node_to_string_al(tokens, &an, buffer, 100);
        //CABOR_LOG_TRACE_F("%s", buffer);
    }

    cabor_ast_node* plus = cabor_access_ast_node(&ast);
    cabor_ast_node* a = cabor_access_ast_node(&cabor_access_ast_node(&ast)->edges[0]);
    cabor_ast_node* star = cabor_access_ast_node(&cabor_access_ast_node(&ast)->edges[1]);
    cabor_ast_node* b = cabor_access_ast_node(&star->edges[0]);
    cabor_ast_node* c = cabor_access_ast_node(&star->edges[1]);

    cabor_token* plus_t = cabor_vector_get_token(tokens, plus->token_index);
    cabor_token* a_t = cabor_vector_get_token(tokens, a->token_index);
    cabor_token* star_t = cabor_vector_get_token(tokens, star->token_index);
    cabor_token* b_t = cabor_vector_get_token(tokens, b->token_index);
    cabor_token* c_t = cabor_vector_get_token(tokens, c->token_index);

    CABOR_CHECK_EQUALS(plus_t->data[0], '+', res);
    CABOR_CHECK_EQUALS(a_t->data[0], 'a', res);
    CABOR_CHECK_EQUALS(star_t->data[0], '*', res);
    CABOR_CHECK_EQUALS(b_t->data[0], 'b', res);
    CABOR_CHECK_EQUALS(c_t->data[0], 'c', res);

    cabor_destroy_vector(ast_nodes);
    cabor_free_ast(&ast);

    cabor_destroy_vector(tokens);

    return res;
}

// Try to parse expression: c * b + a
int cabor_test_parse_expression_cba()
{
    cabor_vector* tokens = cabor_create_vector(100, CABOR_TOKEN, true);

    cabor_token tmp[5] = { 0 };
    size_t i = 0;

    tmp[i++] = create_token("c", CABOR_IDENTIFIER);
    tmp[i++] = create_token("*", CABOR_OPERATOR);
    tmp[i++] = create_token("b", CABOR_IDENTIFIER);
    tmp[i++] = create_token("+", CABOR_OPERATOR);
    tmp[i++] = create_token("a", CABOR_IDENTIFIER);

    for (size_t i = 0; i < 5; i++)
    {
        cabor_vector_push_token(tokens, &tmp[i]);
    }

    char token_string[100] = { 0 };

    cabor_stringify_tokens(token_string, 100, tokens);

    const char* expected_tokens = "['c', '*', 'b', '+', 'a']";

    int test = strcmp(expected_tokens, token_string);

    if (test)
    {
        CABOR_LOG_ERR_F("EXPECTED: %s", expected_tokens);
        CABOR_LOG_ERR_F("ACTUAL  : %s", token_string);
    }

    int res = 0;
    CABOR_CHECK_EQUALS(test, 0, res);

    size_t cursor = 0;
    cabor_ast_allocated_node ast = cabor_parse_expression(tokens, &cursor);
    cabor_vector* ast_nodes = cabor_get_ast_node_list_al(&ast);

    char buffer[100] = { 0 };

    for (size_t i = 0; i < ast_nodes->size; i++)
    {
        cabor_ast_node* n = cabor_vector_get_ptr(ast_nodes, i);
        cabor_ast_allocated_node an =
        {
            .node_mem.mem = n,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .node_mem.size = sizeof(cabor_ast_node),
#endif
        };
        memset(buffer, 0, 100);
        cabor_ast_node_to_string_al(tokens, &an, buffer, 100);
        //CABOR_LOG_TRACE_F("%s", buffer);
    }

    cabor_ast_node* plus = cabor_access_ast_node(&ast);
    cabor_ast_node* star = cabor_access_ast_node(&plus->edges[0]);
    cabor_ast_node* a = cabor_access_ast_node(&plus->edges[1]);
    cabor_ast_node* c = cabor_access_ast_node(&star->edges[0]);
    cabor_ast_node* b = cabor_access_ast_node(&star->edges[1]);

    cabor_token* plus_t = cabor_vector_get_token(tokens, plus->token_index);
    cabor_token* a_t = cabor_vector_get_token(tokens, a->token_index);
    cabor_token* star_t = cabor_vector_get_token(tokens, star->token_index);
    cabor_token* b_t = cabor_vector_get_token(tokens, b->token_index);
    cabor_token* c_t = cabor_vector_get_token(tokens, c->token_index);

    CABOR_CHECK_EQUALS(plus_t->data[0], '+', res);
    CABOR_CHECK_EQUALS(a_t->data[0], 'a', res);
    CABOR_CHECK_EQUALS(star_t->data[0], '*', res);
    CABOR_CHECK_EQUALS(b_t->data[0], 'b', res);
    CABOR_CHECK_EQUALS(c_t->data[0], 'c', res);

    cabor_destroy_vector(ast_nodes);
    cabor_free_ast(&ast);

    cabor_destroy_vector(tokens);

    return res;
}

// Try to parse expression (a + b) * c
int cabor_test_parse_expression_abc_parenthesized()
{
    cabor_vector* tokens = cabor_create_vector(100, CABOR_TOKEN, true);

    cabor_token tmp[7] = { 0 };
    size_t i = 0;

    tmp[i++] = create_token("(", CABOR_PUNCTUATION);
    tmp[i++] = create_token("a", CABOR_IDENTIFIER);
    tmp[i++] = create_token("+", CABOR_OPERATOR);
    tmp[i++] = create_token("b", CABOR_IDENTIFIER);
    tmp[i++] = create_token(")", CABOR_PUNCTUATION);
    tmp[i++] = create_token("*", CABOR_OPERATOR);
    tmp[i++] = create_token("c", CABOR_IDENTIFIER);

    for (size_t i = 0; i < 7; i++)
    {
        cabor_vector_push_token(tokens, &tmp[i]);
    }

    char token_string[100] = { 0 };

    cabor_stringify_tokens(token_string, 100, tokens);

    const char* expected_tokens = "['(', 'a', '+', 'b', ')', '*', 'c']";

    int test = strcmp(expected_tokens, token_string);

    if (test)
    {
        CABOR_LOG_ERR_F("EXPECTED: %s", expected_tokens);
        CABOR_LOG_ERR_F("ACTUAL  : %s", token_string);
    }

    int res = 0;
    CABOR_CHECK_EQUALS(test, 0, res);

    size_t cursor = 0;
    cabor_ast_allocated_node ast = cabor_parse_expression(tokens, &cursor);
    cabor_vector* ast_nodes = cabor_get_ast_node_list_al(&ast);

    char buffer[100] = { 0 };

    for (size_t i = 0; i < ast_nodes->size; i++)
    {
        cabor_ast_node* n = cabor_vector_get_ptr(ast_nodes, i);
        cabor_ast_allocated_node an =
        {
            .node_mem.mem = n,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .node_mem.size = sizeof(cabor_ast_node),
#endif
        };
        memset(buffer, 0, 100);
        cabor_ast_node_to_string_al(tokens, &an, buffer, 100);
        //CABOR_LOG_TRACE_F("%s", buffer);
    }

    cabor_ast_node* star = cabor_access_ast_node(&ast);
    cabor_ast_node* plus = cabor_access_ast_node(&star->edges[0]);
    cabor_ast_node* c = cabor_access_ast_node(&star->edges[1]);
    cabor_ast_node* a = cabor_access_ast_node(&plus->edges[0]);
    cabor_ast_node* b = cabor_access_ast_node(&plus->edges[1]);

    cabor_token* plus_t = cabor_vector_get_token(tokens, plus->token_index);
    cabor_token* a_t = cabor_vector_get_token(tokens, a->token_index);
    cabor_token* star_t = cabor_vector_get_token(tokens, star->token_index);
    cabor_token* b_t = cabor_vector_get_token(tokens, b->token_index);
    cabor_token* c_t = cabor_vector_get_token(tokens, c->token_index);

    CABOR_CHECK_EQUALS(plus_t->data[0], '+', res);
    CABOR_CHECK_EQUALS(a_t->data[0],    'a', res);
    CABOR_CHECK_EQUALS(star_t->data[0], '*', res);
    CABOR_CHECK_EQUALS(b_t->data[0],    'b', res);
    CABOR_CHECK_EQUALS(c_t->data[0],    'c', res);

    cabor_destroy_vector(ast_nodes);
    cabor_free_ast(&ast);

    cabor_destroy_vector(tokens);

    return res;
}

// Try to parse expression if a then b + c else x * y
int cabor_test_parse_expression_if_then_else()
{
    cabor_vector* tokens = cabor_create_vector(100, CABOR_TOKEN, true);

    cabor_token tmp[10] = { 0 };
    size_t i = 0;

    tmp[i++] = create_token("if", CABOR_KEYWORD);
    tmp[i++] = create_token("a", CABOR_IDENTIFIER);
    tmp[i++] = create_token("then", CABOR_KEYWORD);
    tmp[i++] = create_token("b", CABOR_IDENTIFIER);
    tmp[i++] = create_token("+", CABOR_OPERATOR);
    tmp[i++] = create_token("c", CABOR_IDENTIFIER);
    tmp[i++] = create_token("else", CABOR_KEYWORD);
    tmp[i++] = create_token("x", CABOR_IDENTIFIER);
    tmp[i++] = create_token("*", CABOR_OPERATOR);
    tmp[i++] = create_token("y", CABOR_IDENTIFIER);

    char buffer[100] = { 0 };

    for (size_t j = 0; j < 10; j++)
        cabor_vector_push_token(tokens, &tmp[j]);

    size_t cursor = 0;
    cabor_ast_allocated_node ast = cabor_parse_if_then_else_expression(tokens, &cursor);
    cabor_vector* ast_nodes = cabor_get_ast_node_list_al(&ast);

    for (size_t i = 0; i < ast_nodes->size; i++)
    {
        cabor_ast_node* n = cabor_vector_get_ptr(ast_nodes, i);
        cabor_ast_allocated_node an =
        {
            .node_mem.mem = n,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .node_mem.size = sizeof(cabor_ast_node),
#endif
        };
        memset(buffer, 0, 100);
        cabor_ast_node_to_string_al(tokens, &an, buffer, 100);
        //CABOR_LOG_TRACE_F("%s", buffer);
    }

    cabor_ast_node* root = cabor_access_ast_node(&ast);
    cabor_ast_node* a = cabor_access_ast_node(&root->edges[0]);
    cabor_ast_node* plus = cabor_access_ast_node(&root->edges[1]);
    cabor_ast_node* star = cabor_access_ast_node(&root->edges[2]);

    cabor_ast_node* b = cabor_access_ast_node(&plus->edges[0]);
    cabor_ast_node* c = cabor_access_ast_node(&plus->edges[1]);

    cabor_ast_node* x = cabor_access_ast_node(&star->edges[0]);
    cabor_ast_node* y = cabor_access_ast_node(&star->edges[1]);

    cabor_token* root_t = cabor_vector_get_token(tokens, root->token_index);
    cabor_token* a_t = cabor_vector_get_token(tokens, a->token_index);
    cabor_token* plus_t = cabor_vector_get_token(tokens, plus->token_index);
    cabor_token* star_t = cabor_vector_get_token(tokens, star->token_index);

    cabor_token* b_t = cabor_vector_get_token(tokens, b->token_index);
    cabor_token* c_t = cabor_vector_get_token(tokens, c->token_index);

    cabor_token* x_t = cabor_vector_get_token(tokens, x->token_index);
    cabor_token* y_t = cabor_vector_get_token(tokens, y->token_index);

    int res = 0;

    CABOR_CHECK_EQUALS(root_t->data[0], 'i', res);
    CABOR_CHECK_EQUALS(root_t->data[1], 'f', res);
    CABOR_CHECK_EQUALS(a_t->data[0],    'a', res);
    CABOR_CHECK_EQUALS(plus_t->data[0], '+', res);
    CABOR_CHECK_EQUALS(star_t->data[0], '*', res);
    CABOR_CHECK_EQUALS(b_t->data[0],    'b', res);
    CABOR_CHECK_EQUALS(c_t->data[0],    'c', res);
    CABOR_CHECK_EQUALS(x_t->data[0],    'x', res);
    CABOR_CHECK_EQUALS(y_t->data[0],    'y', res);

    cabor_destroy_vector(ast_nodes);
    cabor_free_ast(&ast);
    cabor_destroy_vector(tokens);

    return res;
}

// Try to parse expression if a then b + c
int cabor_test_parse_expression_if_then()
{
    cabor_vector* tokens = cabor_create_vector(100, CABOR_TOKEN, true);

    cabor_token tmp[10] = { 0 };
    size_t i = 0;

    tmp[i++] = create_token("if", CABOR_KEYWORD);
    tmp[i++] = create_token("a", CABOR_IDENTIFIER);
    tmp[i++] = create_token("then", CABOR_KEYWORD);
    tmp[i++] = create_token("b", CABOR_IDENTIFIER);
    tmp[i++] = create_token("+", CABOR_OPERATOR);
    tmp[i++] = create_token("c", CABOR_IDENTIFIER);
    char buffer[100] = { 0 };

    for (size_t j = 0; j < 6; j++)
        cabor_vector_push_token(tokens, &tmp[j]);

    size_t cursor = 0;
    cabor_ast_allocated_node ast = cabor_parse_if_then_else_expression(tokens, &cursor);
    cabor_vector* ast_nodes = cabor_get_ast_node_list_al(&ast);

    for (size_t i = 0; i < ast_nodes->size; i++)
    {
        cabor_ast_node* n = cabor_vector_get_ptr(ast_nodes, i);
        cabor_ast_allocated_node an =
        {
            .node_mem.mem = n,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .node_mem.size = sizeof(cabor_ast_node),
#endif
        };
        memset(buffer, 0, 100);
        cabor_ast_node_to_string_al(tokens, &an, buffer, 100);
       //CABOR_LOG_TRACE_F("%s", buffer);
    }

    cabor_ast_node* root = cabor_access_ast_node(&ast);
    cabor_ast_node* a = cabor_access_ast_node(&root->edges[0]);
    cabor_ast_node* plus = cabor_access_ast_node(&root->edges[1]);

    cabor_ast_node* b = cabor_access_ast_node(&plus->edges[0]);
    cabor_ast_node* c = cabor_access_ast_node(&plus->edges[1]);

    cabor_token* root_t = cabor_vector_get_token(tokens, root->token_index);
    cabor_token* a_t = cabor_vector_get_token(tokens, a->token_index);
    cabor_token* plus_t = cabor_vector_get_token(tokens, plus->token_index);

    cabor_token* b_t = cabor_vector_get_token(tokens, b->token_index);
    cabor_token* c_t = cabor_vector_get_token(tokens, c->token_index);
    int res = 0;

    CABOR_CHECK_EQUALS(root_t->data[0], 'i', res);
    CABOR_CHECK_EQUALS(root_t->data[1], 'f', res);
    CABOR_CHECK_EQUALS(a_t->data[0],    'a', res);
    CABOR_CHECK_EQUALS(plus_t->data[0], '+', res);
    CABOR_CHECK_EQUALS(b_t->data[0],    'b', res);
    CABOR_CHECK_EQUALS(c_t->data[0],    'c', res);

    cabor_destroy_vector(ast_nodes);
    cabor_free_ast(&ast);
    cabor_destroy_vector(tokens);

    return res;
}

// Parse hello(a, b, c)
int cabor_test_parse_function_hello()
{
    cabor_vector* tokens = cabor_create_vector(100, CABOR_TOKEN, true);

    cabor_token tmp[10] = { 0 };
    size_t i = 0;

    tmp[i++] = create_token("hello", CABOR_IDENTIFIER);
    tmp[i++] = create_token("(", CABOR_PUNCTUATION);
    tmp[i++] = create_token("a", CABOR_IDENTIFIER);
    tmp[i++] = create_token(",", CABOR_PUNCTUATION);
    tmp[i++] = create_token("b", CABOR_IDENTIFIER);
    tmp[i++] = create_token(",", CABOR_PUNCTUATION);
    tmp[i++] = create_token("c", CABOR_IDENTIFIER);
    tmp[i++] = create_token(")", CABOR_PUNCTUATION);
    char buffer[100] = { 0 };

    for (size_t j = 0; j < 8; j++)
        cabor_vector_push_token(tokens, &tmp[j]);

    size_t cursor = 0;
    cabor_ast_allocated_node ast = cabor_parse_function(tokens, &cursor);
    cabor_vector* ast_nodes = cabor_get_ast_node_list_al(&ast);

    for (size_t i = 0; i < ast_nodes->size; i++)
    {
        cabor_ast_node* n = cabor_vector_get_ptr(ast_nodes, i);
        cabor_ast_allocated_node an =
        {
            .node_mem.mem = n,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .node_mem.size = sizeof(cabor_ast_node),
#endif
        };
        memset(buffer, 0, 100);
        cabor_ast_node_to_string_al(tokens, &an, buffer, 100);
       //CABOR_LOG_TRACE_F("%s", buffer);
    }

    cabor_ast_node* root = cabor_access_ast_node(&ast);
    cabor_ast_node* a = cabor_access_ast_node(&root->edges[0]);
    cabor_ast_node* b = cabor_access_ast_node(&root->edges[1]);
    cabor_ast_node* c = cabor_access_ast_node(&root->edges[2]);

    cabor_token* root_t = cabor_vector_get_token(tokens, root->token_index);
    cabor_token* a_t = cabor_vector_get_token(tokens, a->token_index);
    cabor_token* b_t = cabor_vector_get_token(tokens, b->token_index);
    cabor_token* c_t = cabor_vector_get_token(tokens, c->token_index);

    int res = 0;

    CABOR_CHECK_EQUALS(strcmp(root_t->data, "hello"), 0, res);
    CABOR_CHECK_EQUALS(a_t->data[0], 'a', res);
    CABOR_CHECK_EQUALS(b_t->data[0], 'b', res);
    CABOR_CHECK_EQUALS(c_t->data[0], 'c', res);


    cabor_destroy_vector(ast_nodes);
    cabor_free_ast(&ast);
    cabor_destroy_vector(tokens);

    return res;
}

// Integration tests: tokenizer + parser

int cabor_integration_test_parser_common(const char* code, const char** expected, size_t node_count, cabor_ast_allocated_node(top_level_parser)(cabor_vector* tokens, size_t* cursor))
{
    int res = 0;
    cabor_file* file = cabor_file_from_buffer(code, strlen(code));
    cabor_vector* tokens = cabor_tokenize(file);
    cabor_destroy_file(file);

    size_t cursor = 0;
    cabor_ast_allocated_node ast = top_level_parser(tokens, &cursor);
    cabor_vector* nodes = cabor_get_ast_node_list_al(&ast);
    CABOR_CHECK_EQUALS(nodes->size, node_count, res);
    for (size_t i = 0; i < node_count; i++)
    {
        char buffer[128] = {0};
        cabor_ast_node_to_string(tokens, cabor_vector_get_ptr(nodes, i), buffer, 128);
        int comp = strcmp(buffer, expected[i]);
        //CABOR_LOG(buffer);
        if (comp != 0)
        {
            CABOR_LOG_ERR_F("EXPECTED: %s", expected[i]);
            CABOR_LOG_ERR_F("RECEIVED: %s", buffer);
        }
        CABOR_CHECK_EQUALS(comp, 0, res);
    }

    cabor_destroy_vector(nodes);
    cabor_destroy_vector(tokens);
    cabor_free_ast(&ast);

    return res;
}

// The order for these strings comes from DFS, it takes the right-most branch first

int cabor_integration_test_parse_expression_abc()
{
    const char* code = "a + b * c";
    const char* expected[] = 
    {
        "root: +, edges: ['a', '*']",
        "root: *, edges: ['b', 'c']",
        "root: c, edges: []",
        "root: b, edges: []",
        "root: a, edges: []"
    };
    return cabor_integration_test_parser_common(code, expected, 5, cabor_parse_expression);
}

int cabor_integration_test_parse_expression_cba()
{
    const char* code = "c * b + a";
    const char* expected[] = 
    {
        "root: +, edges: ['*', 'a']",
        "root: a, edges: []",
        "root: *, edges: ['c', 'b']",
        "root: b, edges: []",
        "root: c, edges: []",
    };
    return cabor_integration_test_parser_common(code, expected, 5, cabor_parse_expression);
}

int cabor_integration_test_parse_expression_abc_parenthesized()
{
    const char* code = "(a + b) * c";
    const char* expected[] =
    {
        "root: *, edges: ['+', 'c']",
        "root: c, edges: []",
        "root: +, edges: ['a', 'b']",
        "root: b, edges: []",
        "root: a, edges: []",
    };
    return cabor_integration_test_parser_common(code, expected, 5, cabor_parse_expression);
}

int cabor_integration_test_parse_expression_if_then_else()
{
    const char* code = "if a then b + c else x * y";
    const char* expected[] =
    {
        "root: if, edges: ['a', '+', '*']",
        "root: *, edges: ['x', 'y']",
        "root: y, edges: []",
        "root: x, edges: []",
        "root: +, edges: ['b', 'c']",
        "root: c, edges: []",
        "root: b, edges: []",
        "root: a, edges: []",
    };
    return cabor_integration_test_parser_common(code, expected, 8, cabor_parse_if_then_else_expression);
}

int cabor_integration_test_parse_expression_if_then()
{
    const char* code = "if a then b + c";
    const char* expected[] =
    {
        "root: if, edges: ['a', '+']",
        "root: +, edges: ['b', 'c']",
        "root: c, edges: []",
        "root: b, edges: []",
        "root: a, edges: []",
    };
    return cabor_integration_test_parser_common(code, expected, 5, cabor_parse_if_then_else_expression);
}

#endif // CABOR_ENABLE_TESTING
