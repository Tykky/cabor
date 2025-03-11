#include "ir.h"
#include <string.h>
#include "../logging/logging.h"

#define ROOT(n) cabor_access_ast_node(n)
#define TOKEN(n) cabor_access_ast_token(ast, n)
#define EDGE(n, e) cabor_access_ast_node(&n->edges[e])

#define IR_ENTRY(symtab, str) cabor_get_ir_var_entry(symtab, str)
#define IR_VAR_IDX(ir_data, idx) cabor_vector_get_ir_var(ir_data->ir_vars, idx)
#define IR_VAR_KEY(entry) entry->key
#define IR_VAR_TYPE(entry) (cabor_type)entry->value

cabor_ir_data* cabor_create_ir_data()
{
    CABOR_NEW(cabor_ir_data, ir_data);
    ir_data->ir_vars = cabor_create_vector(1024, CABOR_IR_VAR, false);
    ir_data->ir_var_types = cabor_create_hash_map(1024);
    ir_data->ir_labels = cabor_create_vector(1024, CABOR_IR_LABEL, false);
    ir_data->ir_call_args = cabor_create_vector(1024, CABOR_INT, false);
    ir_data->ir_symtab = cabor_create_symbol_table();
    return ir_data;
}

void cabor_destroy_ir_data(cabor_ir_data* ir_data)
{
    cabor_destroy_vector(ir_data->ir_vars);
    cabor_destroy_hash_map(ir_data->ir_var_types);
    cabor_destroy_vector(ir_data->ir_labels);
    cabor_destroy_vector(ir_data->ir_call_args);
    cabor_destroy_symbol_table(ir_data->ir_symtab);
    CABOR_DELETE(cabor_ir_data, ir_data);
}

int cabor_create_ir_var(cabor_ir_data* ir_data, const char* var)
{
    int idx = (int)ir_data->ir_vars->size;
    cabor_ir_var ir_var = { .name = var, .id = idx };
    cabor_vector_push_ir_var(ir_data->ir_vars, &ir_var);
    return idx;
}

int cabor_create_ir_label(cabor_ir_data* ir_data, const char* label)
{
    int idx = (int)ir_data->ir_labels = label;
    cabor_ir_label ir_label = { .name = label };
    cabor_vector_push_ir_label(ir_data->ir_labels, &ir_label);
}

int cabor_create_ir_load_bool_const(cabor_ir_data* ir_data, bool value, int dest)
{
    int idx = (int)ir_data->ir_instructions->size;
    cabor_ir_load_bool_const instr = 
    {
        .value = value,
        .dest = dest
    };
    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

int cabor_create_ir_copy(cabor_ir_data* ir_data, int source, int dest)
{
    int idx = (int)ir_data->ir_instructions->size;
    cabor_ir_copy instr = 
    {
        .source = source,
        .dest = dest
    };
    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

int cabor_create_ir_call(cabor_ir_data* ir_data, int fun, int* args, int num_args, int dest)
{
    int idx = (int)ir_data->ir_instructions->size;
    int args_idx = (int)ir_data->ir_call_args->size;
    int* args_begin = cabor_vector_get_int(ir_data->ir_call_args, args_idx);
    cabor_vector_reserve(ir_data->ir_call_args, ir_data->ir_call_args->size + num_args);

    memcpy(args_begin, args, num_args);

    cabor_ir_call instr = 
    {
        .fun = fun,
        .args = args_begin,
        .num_args = num_args,
        .dest = dest
    };

    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

int cabor_create_ir_jump(cabor_ir_data* ir_data, int label)
{
    int idx = (int)ir_data->ir_instructions->size;
    cabor_ir_jump instr = 
    {
        .label = label
    };
    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

int cabor_create_ir_condjump(cabor_ir_data* ir_data, int cond, int then_label, int else_label)
{
    int idx = (int)ir_data->ir_instructions->size;
    cabor_ir_condjump instr = 
    {
        .cond = cond,
        .then_label = then_label,
        .else_label = else_label
    };
    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

cabor_ir_var_entry* cabor_get_ir_var_entry(cabor_symbol_table* sym_tab, const char* ir_var)
{
    bool found = false;
    cabor_map_entry* entry = cabor_vector_get_map_entry(sym_tab, ir_var);
    if (!found)
    {
        CABOR_LOG_ERR_F("IR gen error: failed to get ir var entry for %s", ir_var);
        return NULL;
    }
    return entry;
}

void cabor_generate_ir(cabor_ir_data* ir_data, cabor_ast* ast)
{
    // convert root types to symbtab
    for (size_t i = 0; i < ir_data->ir_var_types->table->size; i++)
    {
        cabor_map_entry* entry = cabor_vector_get_map_entry(ir_data->ir_var_types, i);
        const char* key = entry->key;
        if (key)
        {
            // right now this just needs to exist, we don't care about the value
            cabor_map_insert(ir_data->ir_symtab->map, key, -1);
        }
    }

    cabor_ast_node* root_expr = cabor_access_ast_node(ast->root);

    cabor_ir_var_idx final_var_idx = cabor_visit_ir_node(ir_data, ast, root_expr, ir_data->ir_symtab);
    cabor_ir_var* ir_var = IR_VAR_IDX(ir_data, final_var_idx);
    cabor_ir_var_entry* entry = IR_ENTRY(ir_data->ir_symtab, ir_var->name);
    cabor_type var_type = IR_VAR_TYPE(entry);

    if (var_type == CABOR_TYPE_INT)
    {
        // emit call print_int
    }
    else if (var_type == CABOR_TYPE_BOOL)
    {
        // emit call print_bool
    }

}

cabor_ir_var_idx cabor_visit_ir_binaryop(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_table)
{
    cabor_token* root_t = TOKEN(root_expr);
    cabor_ir_var_entry* entry = IR_ENTRY(root_table, root_t->data);
    cabor_ir_var_idx left = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[0]), root_table);
    cabor_ir_var_idx right = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[1]), root_table);
    // Create new unique ir variable based on type alone
    //cabor_ir_var_idx result = cabor_create_ir_var(ir_data, root_t->data);
}

cabor_ir_var_idx cabor_visit_ir_unaryop(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_literal(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_identifier(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_function_call(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_unit(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_block(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_if_then_else(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_while(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_var_expr(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_declaration(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return 0;
}

cabor_ir_var_idx cabor_visit_ir_node(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    switch (root_expr->node_type)
    {
    case CABOR_NODE_TYPE_BINARY_OP:
        return cabor_visit_ir_binaryop(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_UNARY_OP:
        return cabor_visit_ir_unaryop(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_LITERAL:
        return cabor_visit_ir_literal(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_IDENTIFIER:
        return cabor_visit_ir_identifier(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_FUNCTION_CALL:
        return cabor_visit_ir_function_call(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_UNIT:
        return cabor_visit_ir_unit(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_BLOCK:
        return cabor_visit_ir_block(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_IF_THEN_ELSE:
        return cabor_visit_ir_if_then_else(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_WHILE:
        return cabor_visit_ir_while(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_VAR_EXPR:
        return cabor_visit_ir_var_expr(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_DECLARATION:
        return cabor_visit_ir_declaration(ir_data, ast, root_expr, root_tab);

    case CABOR_NODE_TYPE_UNKNOWN:
        return -1;
    }
    return -1;
}
