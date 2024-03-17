#pragma once

#include "../core/vector.h"
#include "../cabor_defines.h"
#include <stddef.h>

#define CABOR_AST_NODE_MAX_EDGES 10

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
} cabor_ast_node;

typedef struct cabor_ast
{
    cabor_ast_node* root;
    cabor_vector* tokens;
} cabor_ast;


cabor_ast_allocated_node cabor_parse_identifier(cabor_vector* tokens, size_t op_index);
cabor_ast_allocated_node cabor_parse_integer_literal(cabor_vector* tokens, size_t op_index);
cabor_ast_allocated_node cabor_parse_operator(cabor_vector* tokens, size_t op_index, cabor_ast_allocated_node left, cabor_ast_allocated_node right);
cabor_ast_allocated_node cabor_parse_expression(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_parse_term(cabor_vector* tokens, size_t* cursor);
cabor_ast_allocated_node cabor_parse_factor(cabor_vector* tokens, size_t op_index);
cabor_ast_allocated_node cabor_allocate_ast_node(size_t token_index, cabor_ast_node* edges, size_t num_edges);

cabor_vector cabor_get_ast_node_list(cabor_ast_allocated_node* root);

// Prints the root token and neighbors
void cabor_ast_node_to_string(cabor_vector* tokens, cabor_ast_allocated_node* allocated_node, char* buffer, size_t size);

void cabor_free_ast_node(cabor_ast_allocated_node* node);
void cabor_free_ast(cabor_ast_allocated_node* root);

// "unwraps" allocated node into cabor_ast_node
cabor_ast_node* cabor_access_ast_node(cabor_ast_allocated_node* node);

cabor_ast cabor_parse_tokens(cabor_vector* tokens);

