#pragma once

#include "../core/vector.h"
#include "../cabor_defines.h"
#include <stddef.h>

#define CABOR_AST_NODE_MAX_EDGES 10

typedef struct cabor_ast_node
{
    size_t token_index; // index into tokens vector
    struct cabor_ast_node* edges[CABOR_AST_NODE_MAX_EDGES];
} cabor_ast_node;

typedef struct cabor_ast
{
    cabor_ast_node* root;
    cabor_vector* tokens;
} cabor_ast;

cabor_ast cabor_parse_tokens(cabor_vector* tokens);

