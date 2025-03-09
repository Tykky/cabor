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
    table->child_scope = NULL;
    return table;
}

void cabor_destroy_symbol_table(cabor_symbol_table* symbol_table)
{
    cabor_destroy_hash_map(symbol_table->map);
    cabor_symbol_table* next = symbol_table->child_scope;
    while (next)
    {
        cabor_symbol_table* tmp = next;
        next = next->child_scope;
        CABOR_DELETE(cabor_symbol_table, tmp);
    }
    CABOR_DELETE(cabor_symbol_table, symbol_table);
}

cabor_symbol_table* cabor_create_new_symbol_scope(cabor_symbol_table* symbol_table)
{
    cabor_symbol_table* new_table = cabor_create_symbol_table();
    new_table->parent_scope = symbol_table;
    symbol_table->child_scope = new_table;
    return new_table;
}

cabor_type cabor_convert_type_declaration_to_type(cabor_token* type_decl)
{
    if (strcmp(type_decl->data, "Int") == 0)
    {
        return CABOR_TYPE_INT;
    }
    else if (strcmp(type_decl->data, "Bool") == 0)
    {
        return CABOR_TYPE_BOOL;
    }
    else 
    {
        CABOR_LOG_ERR_F("TYPE ERROR: failed to convert %s to cabor_type", type_decl->data);
        return CABOR_TYPE_ERROR;
    }
}

cabor_type cabor_typecheck_if_then_else(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_IF_THEN_ELSE || node->num_edges == 2 || node->num_edges == 3, "not a valid if-then-else node");
    cabor_type if_expr_type = cabor_typecheck(ast, node, sym_table);

    if (if_expr_type != CABOR_TYPE_BOOL)
    {
        CABOR_LOG_ERR("TYPE ERROR: if expression didn't evaluate to bool");
        return CABOR_TYPE_ERROR;
    }

    cabor_type then_expr_type = cabor_typecheck(ast, EDGE(node, 1), sym_table);

    if (node->num_edges == 3)
    {
        cabor_type else_expr_type = cabor_typecheck(ast, EDGE(node, 2), sym_table);

        if (then_expr_type != else_expr_type)
        {
            CABOR_LOG_ERR(" TYPE ERROR: if-then-else branches must have the same type");
        }

        node->type = then_expr_type;
        return then_expr_type;
    }

    node->type = then_expr_type;
    return then_expr_type;
}

cabor_type cabor_typecheck_binary_op(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_BINARY_OP && node->num_edges == 2, "not a valid binary-op node");

    cabor_type left = cabor_typecheck(ast, EDGE(node, 0), sym_table);
    cabor_type right = cabor_typecheck(ast, EDGE(node, 1), sym_table);

    if (left != right)
    {
        CABOR_LOG_ERR("TYPE ERROR: binary op left and right types didn't match");
        return CABOR_TYPE_ERROR;
    }

    node->type = left;
    return left;
}

cabor_type cabor_typecheck_unary_op(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_UNARY_OP && node->num_edges == 1, "not a valid unary-op node");

    cabor_type expr_type = cabor_typecheck(ast, EDGE(node, 0), sym_table);

    if (strcmp(TOKEN(node)->data, "-"))
    {
        if (expr_type != CABOR_TYPE_INT && CABOR_TYPE_UNIT)
        {
            CABOR_LOG_ERR("TYPE ERROR: unary op '-' cannot be used with non number types");
            return CABOR_TYPE_ERROR;
        }
        node->type = expr_type;
        return expr_type;
    }
    else if (strcmp(TOKEN(node)->data, "not"))
    {
        if (expr_type != CABOR_TYPE_BOOL)
        {
            CABOR_LOG_ERR("TYPE ERROR: unary op 'not' cannot be used with non bool operands");
            return CABOR_TYPE_ERROR;
        }
        node->type = CABOR_TYPE_BOOL;
        return CABOR_TYPE_BOOL;
    }
    else
    {
        CABOR_LOG_ERR("TYPE ERROR: unary op was not - or not");
        return CABOR_TYPE_ERROR;
    }
}

cabor_type cabor_typecheck_function(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_FUNCTION_CALL, "not a valid function call");

    // TODO check arguments etc..

    return CABOR_NODE_TYPE_FUNCTION_CALL;
}

cabor_type cabor_typecheck_while(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table)
{
    return 0;
}

cabor_type cabor_typecheck_block(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_BLOCK, "not a valid block");

    cabor_symbol_table* new_scope = cabor_create_new_symbol_scope(sym_table);

    cabor_type last_type = CABOR_TYPE_UNIT;

    for (size_t i = 0; i < node->num_edges; i++)
    {
        last_type = cabor_typecheck(ast, EDGE(node, i), new_scope);
    }

    node->type = last_type;
    return last_type;
}

cabor_type cabor_typecheck_var_expr(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_VAR_EXPR && (node->num_edges == 2 || node->num_edges == 3), "not a valid var expression");

    // Check for type declaration here
    cabor_type initializer_type = cabor_typecheck(ast, EDGE(node, 1), sym_table);

    if (node->num_edges == 3) // includes type declaration
    {
        cabor_ast_node* variable_typedecl_node = EDGE(node, 2);
        cabor_token* variable_typedecl_token = TOKEN(variable_typedecl_node);
        cabor_type variable_typedecl_type = cabor_convert_type_declaration_to_type(variable_typedecl_token);

        if (initializer_type != variable_typedecl_type)
        {
            CABOR_LOG_ERR_F("TYPE ERROR: variable initializer type didn't match type declaration");
            return CABOR_TYPE_ERROR;
        }
        variable_typedecl_node->type = CABOR_TYPE_UNIT;
    }

    // Should we allow shadowing? for now assume we don't do that, let's check if the the identifier already exist

    bool found = false;
    int value = cabor_map_get(sym_table->map, TOKEN(node)->data, &found);

    if (found)
    {
        CABOR_LOG_ERR_F("TYPE ERROR: Double variable declaration with same name: %s", TOKEN(node)->data);
        return CABOR_TYPE_ERROR;
    }

    cabor_ast_node* variable_name_node = EDGE(node, 0);
    cabor_token* variable_name_token = TOKEN(variable_name_node);
    const char* variable_name_str = variable_name_token->data;
    variable_name_node->type = initializer_type;

    // initializer type and declared type should be the same
    cabor_map_insert(sym_table->map, variable_name_str, (int)initializer_type);


    node->type = initializer_type;
    return initializer_type;
}

cabor_type cabor_typecheck_literal(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* symb_table)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_LITERAL && node->num_edges == 0, "not a valid literal");

    if (strcmp(TOKEN(node)->data, "True") == 0 || strcmp(TOKEN(node)->data, "False") == 0)
    {
        node->type = CABOR_TYPE_BOOL;
        return CABOR_TYPE_INT;
    }
    else // Check for valid int literal
    {
        cabor_token* token = TOKEN(node);
        char* c = &token->data[0];
        while (*c)
        {
            if (*c < '0' || *c > '9')
            {
                CABOR_LOG_ERR_F("TYPE ERROR: int literal was not a number: %s", TOKEN(node)->data);
                break;
            }
            ++c;
        }
        node->type = CABOR_TYPE_INT;
        return CABOR_TYPE_INT;
    }
    return CABOR_TYPE_ERROR;
}

cabor_type cabor_typecheck_identifier(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* symb_table)
{
    CABOR_ASSERT(node->node_type == CABOR_NODE_TYPE_IDENTIFIER && node->num_edges == 0, "not a valid identifier");

    // All identifiers should be in sym_table for this scope
    bool found = false;
    cabor_type identifier_type = cabor_map_get(symb_table->map, TOKEN(node)->data, &found);

    if (!found)
    {
        CABOR_LOG_ERR_F("TYPE ERROR: undeclared identifier encountered %s", TOKEN(node)->data);
        return CABOR_TYPE_ERROR;
    }

    node->type = identifier_type;
    return identifier_type;
}

cabor_type cabor_typecheck(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table)
{
    cabor_ast_node* root = node;
    cabor_token* token = TOKEN(root);
    switch (root->node_type)
    {
    case CABOR_NODE_TYPE_BINARY_OP:
        return cabor_typecheck_binary_op(ast, root, sym_table);
        break;

    case CABOR_NODE_TYPE_UNARY_OP:
        return cabor_typecheck_unary_op(ast, root, sym_table);
        break;

    case CABOR_NODE_TYPE_LITERAL:
        return cabor_typecheck_literal(ast, root, sym_table);
        break;

    case CABOR_NODE_TYPE_IDENTIFIER:
        return cabor_typecheck_identifier(ast, root, sym_table);
        break;

    case CABOR_NODE_TYPE_FUNCTION_CALL:
        return cabor_typecheck_function(ast, root, sym_table);
        break;

    case CABOR_NODE_TYPE_UNIT:
        return CABOR_TYPE_UNIT;
        break;

    case CABOR_NODE_TYPE_BLOCK:
        return cabor_typecheck_block(ast, root, sym_table);
        break;

    case CABOR_NODE_TYPE_IF_THEN_ELSE:
        return cabor_typecheck_if_then_else(ast, root, sym_table);
        break;

    case CABOR_NODE_TYPE_WHILE:
        return cabor_typecheck_while(ast, root, sym_table);
        break;

    case CABOR_NODE_TYPE_VAR_EXPR:
        return cabor_typecheck_var_expr(ast, root, sym_table);
        break;

    case CABOR_NODE_TYPE_UNKNOWN:
    default:
        CABOR_LOG_ERR_F("TYPE ERROR: Unknown type encountered in AST, root token: %s", TOKEN(root)->data);
        break;
    }
}

