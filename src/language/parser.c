#include "parser.h"
#include "../core/memory.h"
#include "../core/stack.h"
#include "../debug/cabor_debug.h"
#include "../language/tokenizer.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define CABOR_FUNCTION_PARSER_MAX_ARGS 100

#define IS_VALID_TOKEN(token) token != NULL

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
    return IS_VALID_TOKEN(token) && token->type == CABOR_IDENTIFIER || token->type == CABOR_INTEGER_LITERAL || token->type == CABOR_OPERATOR;
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

cabor_ast_allocated_node cabor_parse_identifier(cabor_vector* tokens, size_t op_index)
{
    CABOR_ASSERT(op_index < tokens->size, "op_index is out of bounds!");
    cabor_ast_allocated_node root_alloc = cabor_allocate_ast_node(op_index, NULL, 0);
    return root_alloc;
}

cabor_ast_allocated_node cabor_parse_integer_literal(cabor_vector* tokens, size_t op_index)
{
    CABOR_ASSERT(op_index < tokens->size, "op_index is out of bounds!");
    cabor_ast_allocated_node root_alloc = cabor_allocate_ast_node(op_index, NULL, 0);
    return root_alloc;
}

cabor_ast_allocated_node cabor_parse_parenthesized(cabor_vector* tokens, size_t* op_index)
{
    cabor_token* begin = cabor_vector_get_token(tokens, *op_index);
    CABOR_ASSERT(begin->data[0] == '(', "Begin token not (");

    ++(*op_index);
    cabor_ast_allocated_node expr = cabor_parse_expression(tokens, op_index);
    ++(*op_index);

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
    cabor_ast_allocated_node root_alloc = cabor_allocate_ast_node(op_index, edges, 2);

    return root_alloc;
}

// Parse + - expression
cabor_ast_allocated_node cabor_parse_expression(cabor_vector* tokens, size_t* cursor)
{
    cabor_ast_allocated_node left = cabor_parse_term(tokens, cursor);

    if (*cursor + 1>= tokens->size)
        return left;

    // lookahead
    cabor_token* next_t = cabor_vector_get_token(tokens, *cursor + 1);

    while (is_plus_minus_operator(next_t))
    {
        cabor_token* op = next(tokens, cursor);
        size_t opi = *cursor;

        next(tokens, cursor);

        cabor_ast_allocated_node right = cabor_parse_term(tokens, cursor);

        left = cabor_parse_operator(tokens, opi, left, right);

        if (*cursor + 1 < tokens->size)
            next_t = cabor_vector_get_token(tokens, *cursor + 1);
        else
            break;
    }

    return left;
}

cabor_ast_allocated_node cabor_parse_if_then_else_expression(cabor_vector* tokens, size_t* cursor)
{
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

    token = cabor_vector_get_token(tokens, *cursor);

    // Fast forward to 'then', parse expression doesn't leave the cursor at the last token but usually second last
    while (!is_then_token(token))
        token = next(tokens, cursor);

    token = next(tokens, cursor); // token after then

    if (!IS_VALID_TOKEN(token)) 
    {
        // No more tokens after then
        CABOR_LOG_ERR("Not enough tokens to parse after then expression!");
        return null_node;
    }

    cabor_ast_allocated_node then_exp = cabor_parse_expression(tokens, cursor);

    // Fast forward to 'else', it's fine if we don't find it since it's optional
    while (IS_VALID_TOKEN(token) && !is_else_token(token))
        token = next(tokens, cursor);

    cabor_ast_allocated_node else_exp;

    if (next(tokens, cursor))
    {
        // Looks like we have more tokens after else;
        else_exp = cabor_parse_expression(tokens, cursor);
        ++edge_count;
    }

    cabor_ast_allocated_node edges[3];
    edges[0] = if_exp;
    edges[1] = then_exp;

    if (edge_count == 3)
        edges[2] = else_exp;

    cabor_ast_allocated_node root = cabor_allocate_ast_node(*cursor, edges, edge_count);
    cabor_access_ast_node(&root)->token_index = root_token_index;

    return root;
}

// Parse * / term
cabor_ast_allocated_node cabor_parse_term(cabor_vector* tokens, size_t* cursor)
{
    cabor_ast_allocated_node left = cabor_parse_factor(tokens, cursor);

    if (*cursor + 1 >= tokens->size)
        return left;

    // lookahead
    cabor_token* next = cabor_vector_get_token(tokens, *cursor + 1);

    while (is_multiply_divide_operator(next))
    {
        ++(*cursor);
        cabor_token* op = cabor_vector_get_token(tokens, *cursor);
        size_t opi = *cursor;
        ++(*cursor);
        cabor_ast_allocated_node right = cabor_parse_factor(tokens, cursor);

        left = cabor_parse_operator(tokens, opi, left, right);

        if (*cursor + 1 < tokens->size)
            next = cabor_vector_get_token(tokens, *cursor + 1);
        else
            break;
    }

    return left;
}

cabor_ast_allocated_node cabor_parse_factor(cabor_vector* tokens, size_t* op_index)
{
    cabor_token* token = cabor_vector_get_token(tokens, *op_index);

    switch (token->type)
    {
        case CABOR_IDENTIFIER:
        {
            return cabor_parse_identifier(tokens, *op_index);
        }
        case CABOR_INTEGER_LITERAL:
        {
            return cabor_parse_integer_literal(tokens, *op_index);
        }
        case CABOR_PUNCTUATION:
        {
            if (token->data[0] == '(' || token->data[0] == ')')
            {
                return cabor_parse_parenthesized(tokens, op_index);
            }
            else
            {
                CABOR_RUNTIME_ERROR("Failed to parse factor! punctuation was not ( or )");
            }
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

        args[argCount] = cabor_parse_expression(tokens, cursor);

        ++argCount;
        token = next(tokens, cursor);

        // If there is only 1 argument
        if (token->data[0] == ')')
        {
            valid = true;
            break;
        }

        // If next is not , we have error
        if (!token->type == CABOR_PUNCTUATION || token->data[0] != ',')
        {
            CABOR_LOG_ERR_F("Expected punctuation , after argument in function but got %s", token->data);
            break;
        }

        token = next(tokens, cursor);
    }

    if (!valid)
    {
        CABOR_LOG_ERR("Failed to parse function");
        return cabor_allocate_ast_node(0, NULL, 0);
    }

    cabor_ast_allocated_node function_root = cabor_allocate_ast_node(function_name_token_idx, args, argCount);

    return function_root;
}


cabor_ast_allocated_node cabor_allocate_ast_node(size_t token_index, cabor_ast_allocated_node* edges, size_t num_edges)
{
    cabor_ast_allocated_node allocated_node =
    {
        .node_mem = CABOR_MALLOC(sizeof(cabor_ast_node))
    };

    cabor_ast_node* node = cabor_access_ast_node(&allocated_node);

    node->token_index = token_index;
    
    if (edges != NULL)
    {
        CABOR_ASSERT(num_edges > 0, "num_edges was zero when edges was not null!");

        // Copy edges to the freshly allocated node
        for (size_t i = 0; i < num_edges; i++)
        {
            node->edges[i] = edges[i];
        };

        node->num_edges = num_edges;
    }
    else
    {
        CABOR_ASSERT(num_edges == 0, "num_edges was greater than zero when edges was null!");
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
            for (size_t i = 0; i < node->num_edges; i++)
            {
                cabor_ast_node* neighbour = cabor_access_ast_node(&node->edges[i]);
                cabor_stack_push(stack, (void*) neighbour);
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
    return cabor_get_ast_node_list_al(root);
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


cabor_ast_allocated_node cabor_parse_tokens(cabor_vector* tokens)
{
    size_t cursor = 0;
    return cabor_parse_expression(tokens, &cursor);
}
