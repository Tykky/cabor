#include "parser.h"
#include "../core/memory.h"
#include "../core/stack.h"
#include "../debug/cabor_debug.h"
#include "../language/tokenizer.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define CABOR_FUNCTION_PARSER_MAX_ARGS 100
#define CABOR_MAX_BINARY_OPS_PER_PRECEDENCE_LEVEL 10
#define CABOR_LAST_BINARY_PRECEDENCE_LEVEL 6
#define CABOR_MAX_BLOCK_EDGES 1000 // This determines maximum number of lines of code ended by ; inside a block

#define IS_VALID_TOKEN(token) token != NULL
#define IS_VALID_NODE(node) node.node_mem.mem != NULL

#define NULL_AST (cabor_ast_allocated_node) { .node_mem.mem = NULL }

typedef struct 
{
    size_t numOps;
    const char* ops[CABOR_MAX_BINARY_OPS_PER_PRECEDENCE_LEVEL];
} binary_precedence_level;

static const binary_precedence_level binary_precedence_levels[] =
{
    {1, "="},
    {1, "or"},
    {1, "and"},
    {2, "==", "!="},
    {4, "<", "<=", ">", ">="},
    {2, "+", "-"},
    {3, "*", "/", "%"},
};

static cabor_token* next(cabor_vector* tokens, size_t* cursor)
{
    size_t next_pos = *cursor + 1;
    if (next_pos < tokens->size)
    {
        return cabor_vector_get_token(tokens, ++(*cursor));
    }
    else
    {
        return NULL;
    }
}

static bool token_is_term(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_IDENTIFIER || token->type == CABOR_INTEGER_LITERAL || token->type == CABOR_OPERATOR || token->type == CABOR_KEYWORD;
}

static bool is_if_token(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_KEYWORD && strcmp(token->data, "if") == 0;
}

static bool is_then_token(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_KEYWORD && strcmp(token->data, "then") == 0;
}

static bool is_else_token(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_KEYWORD && strcmp(token->data, "else") == 0;
}

static bool is_plus_minus_operator(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_OPERATOR && (token->data[0] == '+' || token->data[0] == '-');
}

static bool is_multiply_divide_operator(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_OPERATOR && (token->data[0] == '*' || token->data[0] == '/');
}

static bool is_while_token(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_KEYWORD && (strcmp(token->data, "while") == 0);
}

static bool is_do_token(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_KEYWORD && (strcmp(token->data, "do") == 0);
}

static bool is_var_token(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_KEYWORD && (strcmp(token->data, "var") == 0);
}

static bool is_token_beginning_of_block(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_PUNCTUATION && (strcmp(token->data, "{") == 0);
}

static bool is_token_ending_of_block(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_PUNCTUATION && (strcmp(token->data, "}") == 0);
}


static bool is_token_semicolon(cabor_token* token)
{
    return IS_VALID_TOKEN(token) && token->type == CABOR_PUNCTUATION && (strcmp(token->data, ";") == 0);
}

static bool is_binary_op_at_current_precedence_level(cabor_token* token, size_t current_level)
{
    CABOR_ASSERT(current_level <= CABOR_LAST_BINARY_PRECEDENCE_LEVEL, "Current level is greater than last precedence level");
    if (IS_VALID_TOKEN(token) && token->type == CABOR_OPERATOR)
    {
        binary_precedence_level current = binary_precedence_levels[current_level];
        for (size_t i = 0; i < current.numOps; i++)
        {
            char* op = current.ops[i];
            if (strcmp(op, token->data) == 0)
                return true;
        }
    }
    return false;
}

cabor_ast* cabor_parse(cabor_vector* tokens)
{
    CABOR_NEW(cabor_ast, ast);
    size_t cursor = 0;
    ast->tokens = tokens;
    *ast->root = cabor_parse_expression(tokens, &cursor);
    return ast;
}

void cabor_destroy_ast(cabor_ast* ast)
{
    cabor_free_ast(ast->root);
    CABOR_DELETE(cabor_ast, ast);
}

cabor_token* cabor_access_ast_token(const cabor_ast* ast, const cabor_ast_node* node)
{
    return cabor_vector_get_token(ast->tokens, node->token_index);
}

cabor_token* cabor_access_ast_token_edge(const cabor_ast* ast, const cabor_ast_allocated_node* node, int edge_index)
{
    cabor_ast_node* n = cabor_access_ast_node(node);
    cabor_ast_allocated_node edge_alloc = n->edges[edge_index];
    cabor_ast_node* edge = cabor_access_ast_node(&edge_alloc);
    CABOR_ASSERT(edge_index < n->num_edges, "ast node edge overflow");
    return cabor_access_ast_token(ast, edge);
}

cabor_ast_allocated_node cabor_parse_block(cabor_vector* tokens, size_t* cursor)
{
    cabor_token* token = cabor_vector_get_token(tokens, *cursor);
    if (!is_token_beginning_of_block(token))
    {
        CABOR_LOG_ERR_F("Expected token { but got %s", token->data);
        return NULL_AST;
    }

    size_t block_begin_index = *cursor;

    bool error = false;

    cabor_ast_allocated_node edges[CABOR_MAX_BLOCK_EDGES];
    size_t edge_idx = 0;

    while (IS_VALID_TOKEN(token))
    {
        token = next(tokens, cursor);

        cabor_ast_allocated_node expr = cabor_parse_expression(tokens, cursor);
        if (IS_VALID_NODE(expr))
        {
            edges[edge_idx++] = expr;
        }
        else
        {
            CABOR_LOG_ERR("Got null ast node when attempting to parse expression inside block");
            error = true;
            break;
        }

        token = next(tokens, cursor);
        bool is_ending_of_block = is_token_ending_of_block(token);
        bool is_semicolon = is_token_semicolon(token);

        if (!is_ending_of_block && !is_semicolon)
        {
            CABOR_LOG_ERR_F("Expected '}' or ';' after expression in block but got %s", token->data);
            error = true;
            break;
        }

        if (is_semicolon && !(*cursor + 1 < tokens->size))
        {
            CABOR_LOG_ERR("Expected more tokens after ';' in block but got none!");
            error = true;
            break;
        }

        if (is_semicolon)
        {
            cabor_token* next_t = cabor_vector_get_token(tokens, *cursor + 1);

            // When block is ended by ;} it means the block should evaluate to None
            // When block is ended by only } it means the block should evaluate to value of the last expression
            // To differentiate between the two easily we add unit token as the last edge if we encounter ;}
            if (is_token_ending_of_block(next_t)) 
            {
                cabor_token unit_token = { .type = CABOR_UNIT, .data = "<UNIT>\0"};
                cabor_vector_push_token(tokens, &unit_token);
                size_t unit_token_idx = tokens->size - 1;
                edges[edge_idx++] = cabor_allocate_ast_node(unit_token_idx, NULL, 0, CABOR_NODE_TYPE_UNIT);
                token = next(tokens, cursor);
                break;
            }
        }

        if (is_ending_of_block)
            break;

    }

    // Expect }
    if (!is_token_ending_of_block(token))
    {
        CABOR_LOG_ERR_F("Expected token } but got %s", token->data);
        error = true;
    }

    cabor_ast_allocated_node block;

    if (error)
    {
        block = NULL_AST;
        for (size_t i = 0; i < edge_idx; i++)
        {
            cabor_ast_allocated_node edge = edges[i];
            cabor_free_ast_node(&edge);
        }
    }
    else
    {
        block = cabor_allocate_ast_node(block_begin_index, edges, edge_idx, CABOR_NODE_TYPE_BLOCK);
    }

    return block;
}

// Parse unary '-' and 'not'
cabor_ast_allocated_node cabor_parse_unary(cabor_vector* tokens, size_t* cursor)
{
    size_t op = *cursor;
    next(tokens, cursor);

    cabor_ast_allocated_node operand = cabor_parse_factor(tokens, cursor);
    cabor_ast_allocated_node edges[] = { operand };

    return cabor_allocate_ast_node(op, edges, 1, CABOR_NODE_TYPE_UNARY_OP);
}

cabor_ast_allocated_node cabor_parse_identifier(cabor_vector* tokens, size_t op_index)
{
    CABOR_ASSERT(op_index < tokens->size, "op_index is out of bounds!");
    cabor_ast_allocated_node root_alloc = cabor_allocate_ast_node(op_index, NULL, 0, CABOR_NODE_TYPE_IDENTIFIER);
    return root_alloc;
}

cabor_ast_allocated_node cabor_parse_integer_literal(cabor_vector* tokens, size_t op_index)
{
    CABOR_ASSERT(op_index < tokens->size, "op_index is out of bounds!");
    cabor_ast_allocated_node root_alloc = cabor_allocate_ast_node(op_index, NULL, 0, CABOR_NODE_TYPE_LITERAL);
    return root_alloc;
}

cabor_ast_allocated_node cabor_parse_parenthesized(cabor_vector* tokens, size_t* op_index)
{
    cabor_token* begin = cabor_vector_get_token(tokens, *op_index);
    CABOR_ASSERT(begin->data[0] == '(', "Begin token not (");

    next(tokens, op_index);
    cabor_ast_allocated_node expr = cabor_parse_binary_expression(tokens, op_index, 0);
    next(tokens, op_index);

    cabor_token* end = cabor_vector_get_token(tokens, *op_index);
    CABOR_ASSERT(end->data[0] == ')', "End token not )");

    return expr;
}

cabor_ast_allocated_node cabor_parse_operator(cabor_vector* tokens, size_t op_index, cabor_ast_allocated_node left, cabor_ast_allocated_node right)
{
    cabor_token* left_token = cabor_vector_get_token(tokens, cabor_access_ast_node(&left)->token_index);
    cabor_token* right_token = cabor_vector_get_token(tokens, cabor_access_ast_node(&right)->token_index);
    cabor_token* root_token = cabor_vector_get_token(tokens, op_index);

    CABOR_ASSERT(token_is_term(left_token), "left token not term in expression!");
    CABOR_ASSERT(token_is_term(right_token), "right token not term in expression!");
    CABOR_ASSERT(root_token->type == CABOR_OPERATOR, "root_token token not operator in expression!");

    cabor_ast_allocated_node edges[] = { left, right };
    cabor_ast_allocated_node root_alloc = cabor_allocate_ast_node(op_index, edges, 2, CABOR_NODE_TYPE_BINARY_OP);

    return root_alloc;
}

cabor_ast_allocated_node cabor_parse_binary_expression(cabor_vector* tokens, size_t* cursor, size_t current_precedence_level)
{
    CABOR_ASSERT(*cursor < tokens->size, "cursor overflow");
    cabor_ast_allocated_node left;
    if (current_precedence_level == CABOR_LAST_BINARY_PRECEDENCE_LEVEL)
    {
        left = cabor_parse_factor(tokens, cursor);
    }
    else
    {
        left = cabor_parse_binary_expression(tokens, cursor, current_precedence_level + 1);
    }

    if (*cursor + 1 >= tokens->size)
        return left;

    // lookahead
    cabor_token* next_t = cabor_vector_get_token(tokens, *cursor + 1);

    while (is_binary_op_at_current_precedence_level(next_t, current_precedence_level))
    {
        cabor_token* op = next(tokens, cursor);
        size_t opi = *cursor;

        next(tokens, cursor);

        cabor_ast_allocated_node right;

        if (current_precedence_level == CABOR_LAST_BINARY_PRECEDENCE_LEVEL)
        {
            right = cabor_parse_factor(tokens, cursor);
        }
        else
        {
            right = cabor_parse_binary_expression(tokens, cursor, current_precedence_level + 1);
        }

        left = cabor_parse_operator(tokens, opi, left, right);

        if (*cursor + 1 < tokens->size)
        {
            next_t = cabor_vector_get_token(tokens, *cursor + 1);
        }
        else
            break;
    }

    return left;
}

// Parse + - expression
cabor_ast_allocated_node cabor_parse_expression(cabor_vector* tokens, size_t* cursor)
{
    CABOR_ASSERT(*cursor < tokens->size, "cursor overflow");
    return cabor_parse_binary_expression(tokens, cursor, 0);
}

cabor_ast_allocated_node cabor_parse_if_then_else_expression(cabor_vector* tokens, size_t* cursor)
{
    CABOR_ASSERT(*cursor < tokens->size, "cursor overflow");
    cabor_token* token = cabor_vector_get_token(tokens, *cursor);
    size_t edge_count = 2;

    cabor_ast_allocated_node null_node;
    null_node.node_mem.mem = NULL;

    if (!is_if_token(token))
    {
        return null_node;
    }

    const size_t root_token_index = *cursor;

    if (!next(tokens, cursor)) // Parse expression inside if expression
    {
        CABOR_LOG_ERR("Not enough tokens to parse if expression!");
        return null_node;
    }

    cabor_ast_allocated_node if_exp = cabor_parse_expression(tokens, cursor);

    token = next(tokens, cursor);

    if (!is_then_token(token))
    {
        CABOR_LOG_ERR("Expected 'then' after 'while' but got something else");
        return null_node;
    }

    token = next(tokens, cursor); // token after then

    if (!IS_VALID_TOKEN(token)) 
    {
        // No more tokens after then
        CABOR_LOG_ERR("Not enough tokens to parse after then expression!");
        return null_node;
    }

    cabor_ast_allocated_node then_exp = cabor_parse_expression(tokens, cursor);
    token = next(tokens, cursor);

    // Check for 'else', it's fine if we don't find it since it's optional
    cabor_ast_allocated_node else_exp;
    if (is_else_token(token))
    {
        if (next(tokens, cursor))
        {
            // Looks like we have more tokens after else;
            else_exp = cabor_parse_expression(tokens, cursor);
            ++edge_count;
        }
    }

    cabor_ast_allocated_node edges[3];
    edges[0] = if_exp;
    edges[1] = then_exp;

    if (edge_count == 3)
        edges[2] = else_exp;

    cabor_ast_allocated_node root = cabor_allocate_ast_node(*cursor, edges, edge_count, CABOR_NODE_TYPE_IF_THEN_ELSE);
    cabor_access_ast_node(&root)->token_index = root_token_index;

    return root;
}

cabor_ast_allocated_node cabor_parse_while_expression(cabor_vector* tokens, size_t* cursor)
{
    CABOR_ASSERT(*cursor < tokens->size, "cursor overflow");
    cabor_token* token = cabor_vector_get_token(tokens, *cursor);

    if (!is_while_token(token))
    {
        CABOR_LOG_ERR_F("Expected 'while' token but got %s", token->data);
        return NULL_AST;
    }

    size_t while_token_index = *cursor;
    token = next(tokens, cursor);

    // Parse condition expr
    cabor_ast_allocated_node condition_expr = cabor_parse_expression(tokens, cursor);
    token = next(tokens, cursor);

    if (!is_do_token(token))
    {
        CABOR_LOG_ERR_F("Expected 'do' after 'while' but got %s", token->data);
        return NULL_AST;
    }
    token = next(tokens, cursor);

    cabor_ast_allocated_node do_expr = cabor_parse_expression(tokens, cursor);

    cabor_ast_allocated_node edges[] = { condition_expr, do_expr };
    return cabor_allocate_ast_node(while_token_index, edges, 2, CABOR_NODE_TYPE_WHILE);
}

cabor_ast_allocated_node cabor_parse_var_expression(cabor_vector* tokens, size_t* cursor)
{
    CABOR_ASSERT(*cursor < tokens->size, "cursor overflow");
    cabor_token* token = cabor_vector_get_token(tokens, *cursor);
    if (!is_var_token(token))
    {
        CABOR_LOG_ERR("Expected 'var' token");
        return NULL_AST;
    }

    size_t var_token_index = *cursor;
    token = next(tokens, cursor);

    // Expect variable name
    if (!IS_VALID_TOKEN(token) || token->type != CABOR_IDENTIFIER)
    {
        CABOR_LOG_ERR("Expected identifier after 'var'");
        return NULL_AST;
    }

    size_t identifier_token_index = *cursor;

    token = next(tokens, cursor);

    bool has_type_declaration = false;
    size_t type_declaration_token_index = 0;

    // If there is : after the identifier it means we have the optional type declaration
    if (strcmp(token->data, ":") == 0)
    {
        token = next(tokens, cursor); // this should be the type identifier
        has_type_declaration = true;
        type_declaration_token_index = *cursor;
        token = next(tokens, cursor);
    }

    // expect '=' operator
    if (!IS_VALID_TOKEN(token) || token->type != CABOR_OPERATOR || strcmp(token->data, "=") != 0)
    {
        CABOR_LOG_ERR("Expected '=' after variable name");
        return NULL_AST;
    }

    token = next(tokens, cursor);

    size_t num_edges = has_type_declaration ? 3 : 2;

    cabor_ast_allocated_node assigned_expr = cabor_parse_expression(tokens, cursor);
    cabor_ast_allocated_node edges[3] = { cabor_parse_identifier(tokens, identifier_token_index), assigned_expr };

    if (has_type_declaration)
    {
        edges[2] = cabor_allocate_ast_node(type_declaration_token_index, NULL, 0, CABOR_NODE_TYPE_DECLARATION);
    }

    return cabor_allocate_ast_node(var_token_index, edges, num_edges, CABOR_NODE_TYPE_VAR_EXPR);
}

// Parse * / term
cabor_ast_allocated_node cabor_parse_term(cabor_vector* tokens, size_t* cursor)
{
    return cabor_parse_binary_expression(tokens, cursor, 0);
}

cabor_ast_allocated_node cabor_parse_factor(cabor_vector* tokens, size_t* op_index)
{
    CABOR_ASSERT(*op_index < tokens->size, "op_index is out of bounds!");

    cabor_token* token = cabor_vector_get_token(tokens, *op_index);

    switch (token->type)
    {
    case CABOR_IDENTIFIER:
    {
        if (*op_index + 1 < tokens->size)
        {
            cabor_token* next_token = cabor_vector_get_token(tokens, *op_index + 1);
            if (next_token->type == CABOR_PUNCTUATION && strcmp(next_token->data, "(") == 0)
            {
                return cabor_parse_function(tokens, op_index);
            }
        }
        return cabor_parse_identifier(tokens, *op_index);
    }
    case CABOR_INTEGER_LITERAL:
    {
        return cabor_parse_integer_literal(tokens, *op_index);
    }
    case CABOR_OPERATOR: // parse unary operators '-' and 'not' here
    {
        if (strcmp(token->data, "-") == 0 || strcmp(token->data, "not") == 0)
        {
            return cabor_parse_unary(tokens, op_index);
        }
        break;
    }
    case CABOR_PUNCTUATION:
    {
        if (token->data[0] == '(')
        {
            return cabor_parse_parenthesized(tokens, op_index);
        }
        else if (token->data[0] == '{')
        {
            return cabor_parse_block(tokens, op_index);
        }
        else
        {
            CABOR_RUNTIME_ERROR("Failed to parse factor! punctuation was not (");
        }
        break;
    }
    case CABOR_KEYWORD:
    {
        if (strcmp(token->data, "if") == 0)
        {
            return cabor_parse_if_then_else_expression(tokens, op_index);
        }
        else if (strcmp(token->data, "while") == 0)
        {
            return cabor_parse_while_expression(tokens, op_index);
        }
        else if (strcmp(token->data, "var") == 0)
        {
            return cabor_parse_var_expression(tokens, op_index);
        }
        break;
    }
    default:
        CABOR_RUNTIME_ERROR("Failed to parse factor!");
    }
}

cabor_ast_allocated_node cabor_parse_function(cabor_vector* tokens, size_t* cursor)
{
    cabor_token* token = cabor_vector_get_token(tokens, *cursor);

    // First token should be the function name
    CABOR_ASSERT(token->type == CABOR_IDENTIFIER, "First token in function parser wasn't identifier");

    size_t function_name_token_idx = *cursor;

    token = next(tokens, cursor); // second token should be (
    CABOR_ASSERT(token->type == CABOR_PUNCTUATION, "Second token in function parser wasn't punctuation");

    // Now parse argument list, call expression parser for each arg
    token = next(tokens, cursor);

    cabor_ast_allocated_node args[CABOR_FUNCTION_PARSER_MAX_ARGS];
    size_t argCount = 0;
    bool valid = false;

    while (IS_VALID_TOKEN(token))
    {
        if (token->data[0] == ')')
        {
            valid = true;
            break;
        }

        args[argCount++] = cabor_parse_expression(tokens, cursor);

        bool found_comma = false;
        while (IS_VALID_TOKEN(token)) // we allow expressions inside arg list so each arg can be multiple tokens long
        {
            if (token->data[0] == ',')
            {
                found_comma = true;
                break;
            }

            if (token->data[0] == ')')
            {
                valid = true;
                break;
            }

            token = next(tokens, cursor);
        }

        if (!valid && !found_comma)
        {
            CABOR_LOG_ERR("Expected , in function parser but ran out of tokens");
            break;
        }

        if (token->data[0] == ')')
        {
            break;
        }

        token = next(tokens, cursor);
    }

    if (!valid)
    {
        CABOR_LOG_ERR("Failed to parse function");
        return cabor_allocate_ast_node(0, NULL, 0, CABOR_NODE_TYPE_UNKNOWN);
    }

    cabor_ast_allocated_node* edges = argCount > 0 ? args : NULL;
    cabor_ast_allocated_node function_root = cabor_allocate_ast_node(function_name_token_idx, edges, argCount, CABOR_NODE_TYPE_FUNCTION_CALL);

    return function_root;
}

cabor_ast_allocated_node cabor_allocate_ast_node(size_t token_index, cabor_ast_allocated_node* edges, size_t num_edges, cabor_ast_node_type type)
{
    cabor_ast_allocated_node allocated_node =
    {
        .node_mem = CABOR_MALLOC(sizeof(cabor_ast_node))
    };

    cabor_ast_node* node = cabor_access_ast_node(&allocated_node);

    node->node_type = type;
    node->token_index = token_index;
    
    if (edges != NULL && num_edges > 0)
    {
        // Copy edges to the freshly allocated node
        for (size_t i = 0; i < num_edges; i++)
        {
            node->edges[i] = edges[i];
        };

        node->num_edges = num_edges;
    }
    else
    {
        node->num_edges = 0;
    }

    return allocated_node;
}

static bool is_visited(cabor_vector* nodes, cabor_ast_node* node)
{
    for (size_t i = 0; i < nodes->size; i++)
    {
        if (cabor_vector_get_ptr(nodes, i) == node)
            return true;
    }
    return false;
}

// Get ast tree as list of nodes, note we store node pointers in the list,
// the list does not own the memory for the tree.
cabor_vector* cabor_get_ast_node_list_al(cabor_ast_allocated_node* root)
{
    // Implement DFS

    size_t stack_capacity = 100;
    cabor_stack* stack = cabor_create_stack(stack_capacity);


    cabor_vector* nodes = cabor_create_vector(100, CABOR_PTR, true);
    cabor_ast_node* root_node = cabor_access_ast_node(root);

    cabor_stack_push(stack, root_node);

    while (true)
    {
        cabor_ast_node* node;
        if (!cabor_stack_pop(stack, (void**)&node))
            break; // stack is empty

        if (!is_visited(nodes, node))
        {
            cabor_vector_push_ptr(nodes, node);

            // Add neighbours to stack
            if (node)
            {
                for (size_t i = 0; i < node->num_edges; i++)
                {
                    cabor_ast_node* neighbour = cabor_access_ast_node(&node->edges[i]);
                    cabor_stack_push(stack, (void*)neighbour);
                }
            }
        }
    }

    cabor_destroy_stack(stack);

    return nodes;
}

cabor_vector* cabor_get_ast_node_list(cabor_ast_node* root)
{
    cabor_ast_allocated_node an =
    {
        .node_mem.mem = root,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
        .node_mem.size = sizeof(cabor_ast_node),
#endif
    };
    return cabor_get_ast_node_list_al(&an);
}

void cabor_ast_node_to_string_al(cabor_vector* tokens, cabor_ast_allocated_node* allocated_node, char* buffer, size_t size)
{
    cabor_ast_node* node = cabor_access_ast_node(allocated_node);
    cabor_token* token = cabor_vector_get_token(tokens, node->token_index);
    size_t cursor = 0;
    cursor += snprintf(buffer, size, "root: %s, edges: [", token->data);

    for (size_t i = 0; i < node->num_edges; i++)
    {
        cabor_ast_node* neighbour = cabor_access_ast_node(&node->edges[i]);
        cabor_token* neighbour_token = cabor_vector_get_token(tokens, neighbour->token_index);
        if (i != node->num_edges - 1)
            cursor += snprintf(buffer + cursor, size - cursor, "'%s', ", neighbour_token->data);
        else
            cursor += snprintf(buffer + cursor, size - cursor, "'%s'", neighbour_token->data);
    }

    CABOR_ASSERT(cursor + 1 < size, "out of bounds!");

    buffer[cursor] = ']';
    buffer[cursor + 1] = '\0';
}

void cabor_ast_node_to_string(cabor_vector* tokens, cabor_ast_node* node, char* buffer, size_t size)
{
    cabor_ast_allocated_node an =
    {
        .node_mem.mem = node,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
        .node_mem.size = sizeof(cabor_ast_node),
#endif
    };
    return cabor_ast_node_to_string_al(tokens, &an, buffer, size);
}

// Free only the current node and don't care about freeing any edges
void cabor_free_ast_node(cabor_ast_allocated_node* node)
{
    CABOR_FREE(&node->node_mem);
}

// Free the whole tree
void cabor_free_ast(cabor_ast_allocated_node* root)
{
    cabor_vector* nodes = cabor_get_ast_node_list_al(root);

    for (size_t i = 0; i < nodes->size; i++)
    {
        cabor_ast_node* node = cabor_vector_get_ptr(nodes, i);
        cabor_allocation allocation =
        {
            .mem = node,
#ifdef CABOR_ENABLE_ALLOCATOR_FAT_POINTERS
            .size = sizeof(cabor_ast_node)
#endif
        };

        cabor_ast_allocated_node allocated_node =
        {
            .node_mem = allocation
        };

        cabor_free_ast_node(&allocated_node);
    }
    cabor_destroy_vector(nodes);
}

cabor_ast_node* cabor_access_ast_node(cabor_ast_allocated_node* node)
{
    return (cabor_ast_node*)node->node_mem.mem;
}
