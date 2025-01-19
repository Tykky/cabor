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

// Try to parse expression: a + b * c
int cabor_test_parse_expression_abc()
{
    cabor_vector tokens = cabor_create_vector(100, CABOR_TOKEN, true);

    cabor_token tmp[5] = { 0 };
    size_t i = 0;

    tmp[i++] = create_token("a", CABOR_IDENTIFIER);
    tmp[i++] = create_token("+", CABOR_OPERATOR);
    tmp[i++] = create_token("b", CABOR_IDENTIFIER);
    tmp[i++] = create_token("*", CABOR_OPERATOR);
    tmp[i++] = create_token("c", CABOR_IDENTIFIER);

    for (size_t i = 0; i < 5; i++)
    {
        cabor_vector_push_token(&tokens, &tmp[i]);
    }

    char token_string[100] = { 0 };

    cabor_stringify_tokens(token_string, 100, &tokens);

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
    cabor_ast_allocated_node ast = cabor_parse_expression(&tokens, &cursor);
    cabor_vector ast_nodes = cabor_get_ast_node_list(&ast);

    char buffer[100] = { 0 };

    for (size_t i = 0; i < ast_nodes.size; i++)
    {
        cabor_ast_node* n = cabor_vector_get_ptr(&ast_nodes, i);
        cabor_ast_allocated_node an =
        {
            .node_mem.mem = n,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .node_mem.size = sizeof(cabor_ast_node),
#endif
        };
        memset(buffer, 0, 100);
        cabor_ast_node_to_string(&tokens, &an, buffer, 100);
        //CABOR_LOG_TRACE_F("%s", buffer);
    }

    cabor_ast_node* plus = cabor_access_ast_node(&ast);
    cabor_ast_node* a = cabor_access_ast_node(&cabor_access_ast_node(&ast)->edges[0]);
    cabor_ast_node* star = cabor_access_ast_node(&cabor_access_ast_node(&ast)->edges[1]);
    cabor_ast_node* b = cabor_access_ast_node(&star->edges[0]);
    cabor_ast_node* c = cabor_access_ast_node(&star->edges[1]);

    cabor_token* plus_t = cabor_vector_get_token(&tokens, plus->token_index);
    cabor_token* a_t = cabor_vector_get_token(&tokens, a->token_index);
    cabor_token* star_t = cabor_vector_get_token(&tokens, star->token_index);
    cabor_token* b_t = cabor_vector_get_token(&tokens, b->token_index);
    cabor_token* c_t = cabor_vector_get_token(&tokens, c->token_index);

    CABOR_CHECK_EQUALS(plus_t->data[0], '+', res);
    CABOR_CHECK_EQUALS(a_t->data[0], 'a', res);
    CABOR_CHECK_EQUALS(star_t->data[0], '*', res);
    CABOR_CHECK_EQUALS(b_t->data[0], 'b', res);
    CABOR_CHECK_EQUALS(c_t->data[0], 'c', res);

    destroy_cabor_vector(&ast_nodes);
    cabor_free_ast(&ast);

    destroy_cabor_vector(&tokens);

    return res;
}

// Try to parse expression: c * b + a
int cabor_test_parse_expression_cba()
{
    cabor_vector tokens = cabor_create_vector(100, CABOR_TOKEN, true);

    cabor_token tmp[5] = { 0 };
    size_t i = 0;

    tmp[i++] = create_token("c", CABOR_IDENTIFIER);
    tmp[i++] = create_token("*", CABOR_OPERATOR);
    tmp[i++] = create_token("b", CABOR_IDENTIFIER);
    tmp[i++] = create_token("+", CABOR_OPERATOR);
    tmp[i++] = create_token("a", CABOR_IDENTIFIER);

    for (size_t i = 0; i < 5; i++)
    {
        cabor_vector_push_token(&tokens, &tmp[i]);
    }

    char token_string[100] = { 0 };

    cabor_stringify_tokens(token_string, 100, &tokens);

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
    cabor_ast_allocated_node ast = cabor_parse_expression(&tokens, &cursor);
    cabor_vector ast_nodes = cabor_get_ast_node_list(&ast);

    char buffer[100] = { 0 };

    for (size_t i = 0; i < ast_nodes.size; i++)
    {
        cabor_ast_node* n = cabor_vector_get_ptr(&ast_nodes, i);
        cabor_ast_allocated_node an =
        {
            .node_mem.mem = n,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .node_mem.size = sizeof(cabor_ast_node),
#endif
        };
        memset(buffer, 0, 100);
        cabor_ast_node_to_string(&tokens, &an, buffer, 100);
        //CABOR_LOG_TRACE_F("%s", buffer);
    }

    cabor_ast_node* plus = cabor_access_ast_node(&ast);
    cabor_ast_node* star = cabor_access_ast_node(&plus->edges[0]);
    cabor_ast_node* a = cabor_access_ast_node(&plus->edges[1]);
    cabor_ast_node* c = cabor_access_ast_node(&star->edges[0]);
    cabor_ast_node* b = cabor_access_ast_node(&star->edges[1]);

    cabor_token* plus_t = cabor_vector_get_token(&tokens, plus->token_index);
    cabor_token* a_t = cabor_vector_get_token(&tokens, a->token_index);
    cabor_token* star_t = cabor_vector_get_token(&tokens, star->token_index);
    cabor_token* b_t = cabor_vector_get_token(&tokens, b->token_index);
    cabor_token* c_t = cabor_vector_get_token(&tokens, c->token_index);

    CABOR_CHECK_EQUALS(plus_t->data[0], '+', res);
    CABOR_CHECK_EQUALS(a_t->data[0], 'a', res);
    CABOR_CHECK_EQUALS(star_t->data[0], '*', res);
    CABOR_CHECK_EQUALS(b_t->data[0], 'b', res);
    CABOR_CHECK_EQUALS(c_t->data[0], 'c', res);

    destroy_cabor_vector(&ast_nodes);
    cabor_free_ast(&ast);

    destroy_cabor_vector(&tokens);

    return res;
}

// Try to parse expression (a + b) * c
int cabor_test_parse_expression_abc_parenthesized()
{
    cabor_vector tokens = cabor_create_vector(100, CABOR_TOKEN, true);

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
        cabor_vector_push_token(&tokens, &tmp[i]);
    }

    char token_string[100] = { 0 };

    cabor_stringify_tokens(token_string, 100, &tokens);

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
    cabor_ast_allocated_node ast = cabor_parse_expression(&tokens, &cursor);
    cabor_vector ast_nodes = cabor_get_ast_node_list(&ast);

    char buffer[100] = { 0 };

    for (size_t i = 0; i < ast_nodes.size; i++)
    {
        cabor_ast_node* n = cabor_vector_get_ptr(&ast_nodes, i);
        cabor_ast_allocated_node an =
        {
            .node_mem.mem = n,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .node_mem.size = sizeof(cabor_ast_node),
#endif
        };
        memset(buffer, 0, 100);
        cabor_ast_node_to_string(&tokens, &an, buffer, 100);
        //CABOR_LOG_TRACE_F("%s", buffer);
    }

    cabor_ast_node* star = cabor_access_ast_node(&ast);
    cabor_ast_node* plus = cabor_access_ast_node(&star->edges[0]);
    cabor_ast_node* c = cabor_access_ast_node(&star->edges[1]);
    cabor_ast_node* a = cabor_access_ast_node(&plus->edges[0]);
    cabor_ast_node* b = cabor_access_ast_node(&plus->edges[1]);

    cabor_token* plus_t = cabor_vector_get_token(&tokens, plus->token_index);
    cabor_token* a_t = cabor_vector_get_token(&tokens, a->token_index);
    cabor_token* star_t = cabor_vector_get_token(&tokens, star->token_index);
    cabor_token* b_t = cabor_vector_get_token(&tokens, b->token_index);
    cabor_token* c_t = cabor_vector_get_token(&tokens, c->token_index);

    CABOR_CHECK_EQUALS(plus_t->data[0], '+', res);
    CABOR_CHECK_EQUALS(a_t->data[0], 'a', res);
    CABOR_CHECK_EQUALS(star_t->data[0], '*', res);
    CABOR_CHECK_EQUALS(b_t->data[0], 'b', res);
    CABOR_CHECK_EQUALS(c_t->data[0], 'c', res);

    destroy_cabor_vector(&ast_nodes);
    cabor_free_ast(&ast);

    destroy_cabor_vector(&tokens);

    return res;

}

#endif // CABOR_ENABLE_TESTING
