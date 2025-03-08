#pragma once

#include "../language/tokenizer.h"
#include "../core/vector.h"
#include "../cabor_defines.h"
#include <stddef.h>

#define CABOR_AST_NODE_MAX_EDGES 10

typedef enum
{
    CABOR_NODE_TYPE_BINARY_OP,
    CABOR_NODE_TYPE_UNARY_OP,
    CABOR_NODE_TYPE_LITERAL,
    CABOR_NODE_TYPE_IDENTIFIER,
    CABOR_NODE_TYPE_FUNCTION_CALL,
    CABOR_NODE_TYPE_UNIT,
    CABOR_NODE_TYPE_BLOCK,
    CABOR_NODE_TYPE_IF_THEN_ELSE,
    CABOR_NODE_TYPE_WHILE,
    CABOR_NODE_TYPE_VAR_EXPR,
    CABOR_NODE_TYPE_UNKNOWN
} cabor_ast_node_type;

// same as cabor_ast_node but allocated from heap
typedef struct cabor_ast_allocated_node
{
    // Where we store cabor_ast_node
    cabor_allocation node_mem;
} cabor_ast_allocated_node;

typedef struct cabor_ast_node
{
    size_t token_index; // index into tokens vector
    struct cabor_ast_allocated_node edges[CABOR_AST_NODE_MAX_EDGES];
    size_t num_edges;
    cabor_ast_node_type node_type;
} cabor_ast_node;

typedef struct cabor_ast
{
    cabor_ast_allocated_node* root;
    cabor_vector* tokens;
} cabor_ast;

cabor_ast* cabor_parse(cabor_vector* tokens);
void cabor_destroy_ast(cabor_ast* ast);

// Access token stored inside ast node

cabor_token* cabor_access_ast_token(const cabor_ast* ast, const cabor_ast_node* node);
cabor_token* cabor_access_ast_token_edge(const cabor_ast* ast, const cabor_ast_allocated_node* node, int edge_index);

cabor_ast_allocated_node cabor_parse_block(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_parse_unary(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_parse_identifier(cabor_vector* tokens, size_t op_index);
cabor_ast_allocated_node cabor_parse_integer_literal(cabor_vector* tokens, size_t op_index);
cabor_ast_allocated_node cabor_parse_parenthesized(cabor_vector* tokens, size_t* op_index);
cabor_ast_allocated_node cabor_parse_operator(cabor_vector* tokens, size_t op_index, cabor_ast_allocated_node left, cabor_ast_allocated_node right);
cabor_ast_allocated_node cabor_parse_binary_expression(cabor_vector* tokens, size_t* cursor, size_t current_precedence_level);
cabor_ast_allocated_node cabor_parse_expression(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_parse_if_then_else_expression(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_parse_while_expression(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_parse_var_expression(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_parse_term(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_parse_factor(cabor_vector* tokens, size_t* op_index);
cabor_ast_allocated_node cabor_parse_function(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_allocate_ast_node(size_t token_index, cabor_ast_allocated_node* edges, size_t num_edges, cabor_ast_node_type type);

cabor_vector* cabor_get_ast_node_list_al(cabor_ast_allocated_node* root);
cabor_vector* cabor_get_ast_node_list(cabor_ast_node* root);

// Prints the root token and neighbors
void cabor_ast_node_to_string_al(cabor_vector* tokens, cabor_ast_allocated_node* allocated_node, char* buffer, size_t size);
void cabor_ast_node_to_string(cabor_vector* tokens, cabor_ast_node* node, char* buffer, size_t size);

void cabor_free_ast_node(cabor_ast_allocated_node* node);
void cabor_free_ast(cabor_ast_allocated_node* root);

// "unwraps" allocated node into cabor_ast_node
cabor_ast_node* cabor_access_ast_node(cabor_ast_allocated_node* node);


