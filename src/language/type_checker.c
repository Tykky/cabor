#include "type_checker.h"
#include "../debug/cabor_debug.h"

#include <string.h>
#include <stdbool.h>

#define ROOT(n) cabor_access_ast_node(n)
#define TOKEN(n) cabor_access_ast_token(ast, n)
#define EDGE(n, e) cabor_access_ast_node(&n->edges[e])

cabor_symbol_table* cabor_create_symbol_table()
{
    CABOR_NEW(cabor_symbol_table, table);
    table->map = cabor_create_hash_map(1024);
    table->parent_scope = NULL;
    return table;
}

void cabor_destroy_symbol_table(cabor_symbol_table* symbol_table)
{
    cabor_destroy_hash_map(symbol_table->map);
    CABOR_DELETE(cabor_symbol_table, symbol_table);
}

cabor_type cabor_typecheck_if_then_else(cabor_ast* ast, cabor_ast_node* node)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_IF_THEN_ELSE || node->num_edges == 2 || node->num_edges == 3, "not a valid if-then-else node");
    cabor_type if_expr_type = cabor_typecheck(ast, EDGE(node, 0));

    if (if_expr_type != CABOR_TYPE_BOOL)
    {
        CABOR_LOG_ERR("TYPE ERROR: if expression didn't evaluate to bool");
        return CABOR_TYPE_ERROR;
    }

    cabor_type then_expr_type = cabor_typecheck(ast, EDGE(node, 1));

    if (node->num_edges == 3)
    {
        cabor_type else_expr_type = cabor_typecheck(ast, EDGE(node, 2));

        if (then_expr_type != else_expr_type)
        {
            CABOR_LOG_ERR(" TYPE ERROR: if-then-else branches must have the same type");
        }

        return then_expr_type;
    }

    return then_expr_type;
}

cabor_type cabor_typecheck_binary_op(cabor_ast* ast, cabor_ast_node* node)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_BINARY_OP && node->num_edges == 2, "not a valid binary-op node");

    cabor_type left = cabor_typecheck(ast, node);
    cabor_type right = cabor_typecheck(ast, node);

    if (left != right)
    {
        CABOR_LOG_ERR("TYPE ERROR: binary op left and right types didn't match");
        return CABOR_TYPE_ERROR;
    }

    return left;
}

cabor_type cabor_typecheck_unary_op(cabor_ast* ast, cabor_ast_node* node)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_UNARY_OP && node->num_edges == 1, "not a valid unary-op node");

    cabor_type expr_type = cabor_typecheck(ast, EDGE(node, 0));

    if (strcmp(TOKEN(node)->data, "-"))
    {
        if (expr_type != CABOR_TYPE_INT && CABOR_TYPE_UNIT)
        {
            CABOR_LOG_ERR("TYPE ERROR: unary op '-' cannot be used with non number types");
            return CABOR_TYPE_ERROR;
        }
        return expr_type;
    }
    else if (strcmp(TOKEN(node)->data, "not"))
    {
        if (expr_type != CABOR_TYPE_BOOL)
        {
            CABOR_LOG_ERR("TYPE ERROR: unary op 'not' cannot be used with non bool operands");
            return CABOR_TYPE_ERROR;
        }
        return CABOR_TYPE_BOOL;
    }
    else
    {
        CABOR_LOG_ERR("TYPE ERROR: unary op was not - or not");
        return CABOR_TYPE_ERROR;
    }
}

cabor_type cabor_typecheck_function(cabor_ast* ast, cabor_ast_node* node)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_FUNCTION_CALL, "not a valid function call");

    // TODO check arguments etc..

    return CABOR_NODE_TYPE_FUNCTION_CALL;
}

cabor_type cabor_typecheck_while(cabor_ast* ast, cabor_ast_node* node)
{
    return 0;
}

cabor_type cabor_typecheck_block(cabor_ast* ast, cabor_ast_node* node)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_BLOCK, "not a valid block");

    cabor_type last_type = CABOR_TYPE_UNIT;

    for (size_t i = 0; i < node->num_edges; i++)
    {
        last_type = cabor_typecheck(ast, EDGE(node, i));
    }

    return last_type;
}

cabor_type cabor_typecheck_var_expr(cabor_ast* ast, cabor_ast_node* node)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_VAR_EXPR && node->num_edges == 2, "not a valid var expression");

    // Check for type declaration here
    //cabor_type declared_type = ...
    cabor_type initializer_type = cabor_typecheck(ast, EDGE(node, 1));

    return initializer_type;
}

cabor_type cabor_typecheck(cabor_ast* ast, cabor_vector* tokens)
{
    cabor_ast_node* root = ROOT(ast->root);
    switch (root->node_type)
    {
    case CABOR_NODE_TYPE_BINARY_OP:
        return cabor_typecheck_binary_op(ast, root);
        break;

    case CABOR_NODE_TYPE_UNARY_OP:
        return cabor_typecheck_unary_op(ast, root);
        break;

    case CABOR_NODE_TYPE_LITERAL:
        return CABOR_NODE_TYPE_LITERAL;
        break;

    case CABOR_NODE_TYPE_IDENTIFIER:
        return CABOR_NODE_TYPE_IDENTIFIER;
        break;

    case CABOR_NODE_TYPE_FUNCTION_CALL:
        return cabor_typecheck_function(ast, root);
        break;

    case CABOR_NODE_TYPE_UNIT:
        return CABOR_TYPE_UNIT;
        break;

    case CABOR_NODE_TYPE_BLOCK:
        return cabor_typecheck_block(ast, root);
        break;

    case CABOR_NODE_TYPE_IF_THEN_ELSE:
        return cabor_typecheck_if_then_else(ast, root);
        break;

    case CABOR_NODE_TYPE_WHILE:
        return cabor_typecheck_while(ast, root);
        break;

    case CABOR_NODE_TYPE_VAR_EXPR:
        return cabor_typecheck_var_expr(ast, root);
        break;

    case CABOR_NODE_TYPE_UNKNOWN:
    default:
        CABOR_LOG_ERR("TYPE ERROR: Unknown type encountered in AST");
        break;
    }
}

