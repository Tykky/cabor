#include "ir.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../logging/logging.h"
#include "../debug/cabor_debug.h"

#define ROOT(n) cabor_access_ast_node(n)
#define TOKEN(n) cabor_access_ast_token(ast, n)
#define EDGE(n, e) cabor_access_ast_node(&n->edges[e])

#define IR_ENTRY(symtab, str) cabor_get_ir_var_entry(symtab, str)
#define IR_VAR_IDX(ir_data, idx) cabor_vector_get_ir_var(ir_data->ir_vars, idx)
#define IR_VAR_KEY(entry) entry->key
#define IR_VAR_TYPE(entry) (cabor_type)entry->value

#define CABOR_IR_VAR_INVALID -1
#define CABOR_IR_VAR_UNIT -2

size_t cabor_get_ir_instruction_size()
{
    return sizeof(cabor_ir_instruction);
}

size_t cabor_get_ir_var_size()
{
    return sizeof(cabor_ir_var);
}

size_t cabor_get_ir_label_size()
{
    return sizeof(cabor_ir_label);
}

cabor_ir_data* cabor_create_ir_data()
{
    CABOR_NEW(cabor_ir_data, ir_data);
    ir_data->ir_vars = cabor_create_vector(1024, CABOR_IR_VAR, false);
    ir_data->ir_var_types = cabor_create_hash_map(1024);
    ir_data->ir_labels = cabor_create_vector(1024, CABOR_IR_LABEL, false);
    ir_data->ir_call_args = cabor_create_vector(1024, CABOR_INT, false);
    ir_data->ir_symtab = cabor_create_symbol_table();
    ir_data->ir_instructions = cabor_create_vector(1024, CABOR_IR_INSTRUCTION, false);
    return ir_data;
}

void cabor_destroy_ir_data(cabor_ir_data* ir_data)
{
    cabor_destroy_vector(ir_data->ir_vars);
    cabor_destroy_hash_map(ir_data->ir_var_types);
    cabor_destroy_vector(ir_data->ir_labels);
    cabor_destroy_vector(ir_data->ir_call_args);
    cabor_destroy_symbol_table(ir_data->ir_symtab);
    cabor_destroy_vector(ir_data->ir_instructions);
    CABOR_DELETE(cabor_ir_data, ir_data);
}

cabor_ir_var_idx cabor_create_ir_var(cabor_ir_data* ir_data, const char* var, cabor_type type)
{
    cabor_ir_var_idx idx = (cabor_ir_var_idx)ir_data->ir_vars->size;
    cabor_ir_var ir_var = { .id = idx, .type = type };
    size_t len = strlen(var);

    if (len > CABOR_MAX_IR_VAR_LENGTH)
    {
        CABOR_LOG_ERR_F("IR error: IR var storage was too small for %s", var);
        return CABOR_IR_VAR_INVALID;
    }

    strcpy(ir_var.name, var);

    cabor_vector_push_ir_var(ir_data->ir_vars, &ir_var);

    cabor_map_insert(ir_data->ir_var_types, var, (int)type);

    return idx;
}

cabor_map_entry* cabor_create_ir_var_with_entry(cabor_ir_data* ir_data, const char* var, cabor_type type, cabor_symbol_table* symbtab)
{
    cabor_ir_var_idx idx = (cabor_ir_var_idx)ir_data->ir_vars->size;
    cabor_ir_var ir_var = { .id = idx, .type = type };

    size_t len = strlen(var);
    if (len < CABOR_MAX_IR_VAR_LENGTH)
    {
        strcpy(ir_var.name, var);
    }
    else
    {
        CABOR_LOG_ERR_F("IR error: IR var storage was too small for %s", var);
        return CABOR_IR_VAR_INVALID;
    }

    cabor_vector_push_ir_var(ir_data->ir_vars, &ir_var);

   return cabor_map_insert(symbtab->map, var, (int)idx);
}

cabor_ir_var_idx cabor_create_unique_ir_var(cabor_ir_data* ir_data, cabor_type type)
{
    cabor_ir_var_idx idx = (cabor_ir_var_idx)ir_data->ir_vars->size;
    char buffer[64] = {0};

    int size = snprintf(buffer, sizeof(buffer), "<ir_var>_%d", idx);
    if (size > 64)
    {
        CABOR_RUNTIME_ERROR("snprintf overflow");
    }

    return cabor_create_ir_var(ir_data, buffer, type);
}

cabor_ir_label_idx cabor_create_ir_label(cabor_ir_data* ir_data, const char* label)
{
    cabor_ir_label_idx idx = (cabor_ir_label_idx)ir_data->ir_labels->size;

    cabor_ir_label ir_label;

    size_t len = strlen(label);

    if (len <= CABOR_MAX_LABEL_LENGTH)
    {
        strcpy(ir_label.name, label);
    }
    else
    {
        CABOR_LOG_ERR_F("IR error: label name overflow: %s", label);
    }

    cabor_vector_push_ir_label(ir_data->ir_labels, &ir_label);
    return idx;
}

cabor_ir_inst_idx cabor_push_ir_label(cabor_ir_data* ir_data, cabor_ir_label_idx label)
{
    cabor_ir_inst_idx idx = (cabor_ir_inst_idx)ir_data->ir_instructions->size;
    cabor_ir_instruction instr =
    {
        .type = CABOR_IR_INST_LABEL,
        .label =
        {
            .idx = label 
        }
    };
    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}


cabor_ir_inst_idx cabor_create_ir_load_bool_const(cabor_ir_data* ir_data, bool value, int dest)
{
    cabor_ir_inst_idx idx = (cabor_ir_inst_idx)ir_data->ir_instructions->size;
    cabor_ir_instruction instr =
    {
        .type = CABOR_IR_INST_LOAD_BOOL,
        .load_bool_const =
        {
            .value = value,
            .dest = dest
        }
    };

    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

cabor_ir_inst_idx cabor_create_ir_load_int_const(cabor_ir_data* ir_data, int value, int dest)
{
    cabor_ir_inst_idx idx = (cabor_ir_inst_idx)ir_data->ir_instructions->size;
    cabor_ir_instruction instr =
    {
        .type = CABOR_IR_INST_LOAD_INT,
        .load_int_const =
        {
            .value = value,
            .dest = dest
        }
    };
    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

cabor_ir_inst_idx cabor_create_ir_copy(cabor_ir_data* ir_data, int source, int dest)
{
    cabor_ir_inst_idx idx = (cabor_ir_inst_idx)ir_data->ir_instructions->size;
    cabor_ir_instruction instr = 
    {
        .type = CABOR_IR_INST_COPY,
        .copy = 
        {
            .source = source,
            .dest = dest
        }
    };
    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

cabor_ir_inst_idx cabor_create_ir_call(cabor_ir_data* ir_data, int fun, int* args, int num_args, int dest)
{
    cabor_ir_inst_idx idx = (cabor_ir_inst_idx)ir_data->ir_instructions->size;
    int args_idx = (int)ir_data->ir_call_args->size;
    cabor_vector_reserve(ir_data->ir_call_args, ir_data->ir_call_args->size + num_args);
    int* args_begin = (int*)ir_data->ir_call_args->vector_mem.mem + ir_data->ir_call_args->size;
    ir_data->ir_call_args->size += num_args;

    memcpy(args_begin, args, num_args * sizeof(int));

    cabor_ir_instruction instr = 
    {
        .type = CABOR_IR_INST_CALL,
        .call =
        {
            .fun = fun,
            .args = args_begin,
            .num_args = num_args,
            .dest = dest
        }
    };

    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

cabor_ir_inst_idx cabor_create_ir_jump(cabor_ir_data* ir_data, int label)
{
    cabor_ir_inst_idx idx = (cabor_ir_inst_idx)ir_data->ir_instructions->size;
    cabor_ir_instruction instr = 
    {
        .type = CABOR_IR_INST_JUMP, 
        .jump = 
        {
            .label = label
        }
    };
    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

cabor_ir_inst_idx cabor_create_ir_condjump(cabor_ir_data* ir_data, int cond, int then_label, int else_label)
{
    cabor_ir_inst_idx idx = (cabor_ir_inst_idx)ir_data->ir_instructions->size;
    cabor_ir_instruction instr = 
    {
        .type = CABOR_IR_INST_CONDJUMP,
        .cond_jump = 
        {
            .cond = cond,
            .then_label = then_label,
            .else_label = else_label
        }
    };
    cabor_vector_push_ir_instruction(ir_data->ir_instructions, &instr);
    return idx;
}

cabor_ir_var_entry* cabor_get_ir_var_entry(cabor_symbol_table* sym_tab, const char* ir_var)
{
    bool found = false;
    cabor_map_entry* entry = cabor_map_get_entry(sym_tab->map, ir_var, &found);
    if (!found)
    {
        CABOR_LOG_ERR_F("IR gen error: failed to get ir var entry for %s", ir_var);
        return NULL;
    }
    return entry;
}

void cabor_generate_ir(cabor_ir_data* ir_data, cabor_ast* ast)
{
    cabor_ir_var_idx print_int = cabor_create_ir_var(ir_data, "print_int", CABOR_TYPE_INT);
    cabor_ir_var_idx print_bool = cabor_create_ir_var(ir_data, "print_bool", CABOR_TYPE_BOOL);

    // convert root types to symtab
    for (size_t i = 0; i < ir_data->ir_var_types->table->size; i++)
    {
        cabor_map_entry* entry = cabor_vector_get_map_entry(ir_data->ir_var_types->table, i);
        const char* key = entry->key;
        if (key)
        {
            cabor_map_insert(ir_data->ir_symtab->map, key, -1);
        }
    }

    cabor_map_entry* print_int_entry = cabor_get_ir_var_entry(ir_data->ir_symtab, "print_int");
    cabor_map_entry* print_bool_entry = cabor_get_ir_var_entry(ir_data->ir_symtab, "print_bool");

    print_int_entry->value = print_int;
    print_bool_entry->value = print_bool;

    cabor_ast_node* root_expr = cabor_access_ast_node(ast->root);

    cabor_ir_var_idx final_var_idx = cabor_visit_ir_node(ir_data, ast, root_expr, ir_data->ir_symtab);
    cabor_type var_type;
    if (final_var_idx > 0)
    {
        cabor_ir_var* ir_var = IR_VAR_IDX(ir_data, final_var_idx);
        var_type = ir_var->type;
    }
    else
    {
        var_type = CABOR_TYPE_UNIT;
    }

    if (var_type == CABOR_TYPE_INT)
    {
        // emit call print_int
        cabor_ir_inst_idx inst = cabor_create_ir_call(ir_data, print_int, &final_var_idx, 1, CABOR_IR_VAR_UNIT);
    }
    else if (var_type == CABOR_TYPE_BOOL)
    {
        // emit call print_bool
        cabor_ir_inst_idx inst = cabor_create_ir_call(ir_data, print_bool, &final_var_idx, 1, CABOR_IR_VAR_UNIT);
    }
}

void cabor_format_ir_instruction(cabor_ir_data* ir_data, cabor_ir_inst_idx inst, char* buffer, size_t bufSize)
{
    cabor_ir_instruction* instruction = cabor_vector_get_ir_instruction(ir_data->ir_instructions, inst);
    switch (instruction->type)
    {
    case CABOR_IR_INST_LOAD_BOOL:
    {
        snprintf(buffer, bufSize,
            "LoadBoolConst(%s, x%d)",
            instruction->load_bool_const.value ? "true" : "false",
            instruction->load_bool_const.dest);
        break;
    }

    case CABOR_IR_INST_LOAD_INT:
    {
        snprintf(buffer, bufSize,
            "LoadIntConst(%d, x%d)",
            instruction->load_int_const.value,
            instruction->load_int_const.dest);
        break;
    }

    case CABOR_IR_INST_COPY:
    {
        snprintf(buffer, bufSize,
            "Copy(x%d, x%d)",
            instruction->copy.source,
            instruction->copy.dest);
        break;
    }

    case CABOR_IR_INST_CALL:
    {
        cabor_ir_var* fun_var = cabor_vector_get_ir_var(ir_data->ir_vars, instruction->call.fun);
        int written = snprintf(buffer, bufSize, "Call(%s, [", fun_var->name);

        for (int i = 0; i < instruction->call.num_args; ++i)
        {
            if (written < bufSize)
            {
                written += snprintf(buffer + written, bufSize - written,
                    "%sx%d", i == 0 ? "" : ", ", instruction->call.args[i]);
            }
        }

        if (written < bufSize)
        {
            written += snprintf(buffer + written, bufSize - written,
                "], x%d)", instruction->call.dest);
        }

        break;
    }

    case CABOR_IR_INST_JUMP:
    {
        cabor_ir_label* label = cabor_vector_get_ir_label(ir_data->ir_labels, instruction->jump.label);
        snprintf(buffer, bufSize,
            "Jump(%s)", label->name);
        break;
    }

    case CABOR_IR_INST_CONDJUMP:
    {
        cabor_ir_label* then_label = cabor_vector_get_ir_label(ir_data->ir_labels, instruction->cond_jump.then_label);
        cabor_ir_label* else_label = cabor_vector_get_ir_label(ir_data->ir_labels, instruction->cond_jump.else_label);

        snprintf(buffer, bufSize,
            "CondJump(x%d, %s, %s)",
            instruction->cond_jump.cond,
            then_label->name,
            else_label->name);
        break;
    }

    case CABOR_IR_INST_LABEL:
    {
        cabor_ir_label* label = cabor_vector_get_ir_label(ir_data->ir_labels, instruction->label.idx);
        snprintf(buffer, bufSize,
            "Label(%s)",
            label->name);
        break;
    }

    default:
        snprintf(buffer, bufSize, "<invalid instruction>");
        break;
    }
}

cabor_map_entry* cabor_require_ir_var(cabor_ir_data* ir_data, cabor_symbol_table* symtab, const char* var, cabor_type type)
{
    bool found = false;
    cabor_map_entry* entry = cabor_map_get_entry(symtab->map, var, &found);

    if (!found)
    {
        entry = cabor_create_ir_var_with_entry(ir_data, var, type, symtab);
    }

    return entry;
}

cabor_ir_var_idx cabor_visit_ir_binaryop(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_table)
{
    cabor_token* root_t = TOKEN(root_expr);
    cabor_ir_var_entry* var_op = cabor_require_ir_var(ir_data, root_table, root_t->data, root_expr->type);

    cabor_ir_var_idx left = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[0]), root_table);
    cabor_ir_var_idx right = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[1]), root_table);

    cabor_ir_var_idx var_result = cabor_create_unique_ir_var(ir_data, root_expr->type);

    cabor_ir_var_idx args[] = { left, right };
    cabor_ir_inst_idx inst = cabor_create_ir_call(ir_data, var_op->value, args, 2, var_result);

    return var_result;
}

cabor_ir_var_idx cabor_visit_ir_unaryop(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    cabor_token* token = TOKEN(root_expr);
    const char* op = token->data;

    char unary_op_buf[64];
    snprintf(unary_op_buf, sizeof(unary_op_buf), "unary_%s", op);

    bool found = false;
    cabor_map_entry* fun_entry = cabor_require_ir_var(ir_data, root_tab, unary_op_buf, token->type);
    cabor_ir_var_idx fun = cabor_map_get(root_tab->map, unary_op_buf, &found);
    if (!found)
    {
        CABOR_LOG_ERR_F("IR error: unary operator '%s' not found in symbol table", op);
        return CABOR_IR_VAR_INVALID;
    }

    cabor_ir_var_idx arg = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[0]), root_tab);

    cabor_ir_var_idx result = cabor_create_unique_ir_var(ir_data, root_expr->type);

    cabor_ir_var_idx args[] = { arg };
    cabor_create_ir_call(ir_data, fun, args, 1, result);

    return result;
}

cabor_ir_var_idx cabor_visit_ir_literal(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    cabor_token* token = TOKEN(root_expr);
    cabor_type type = root_expr->type;

    cabor_ir_var_idx var = cabor_create_unique_ir_var(ir_data, type);;

    switch (type)
    {
        case CABOR_TYPE_BOOL:
        {
            bool value;
            if (strcmp(token->data, "true") == 0)
            {
                value = true;
            }
            else if (strcmp(token->data, "false") == 0)
            {
                value = false;
            }
            else
            {
                CABOR_LOG_ERR_F("IR error: bool wasn't 'true' or 'false' it was %s", token->data);
                return CABOR_IR_VAR_INVALID;
            }
            cabor_create_ir_load_bool_const(ir_data, value, var);
            return var;
        }
        case CABOR_TYPE_INT:
        {
            char* endptr;
            long num = strtol(token->data, &endptr, 10);
            if (*endptr != '\0')
            {
                CABOR_LOG_ERR_F("IR error: %s wasn't convertible to int", token->data);
            }
            cabor_create_ir_load_int_const(ir_data, (int)num, var);
            return var;
        }
        case CABOR_TYPE_UNIT:
        {
            return CABOR_TYPE_UNIT;
        }
        default:
        {
            CABOR_LOG_ERR("IR error: literal didn't match to BOO, INT, or UNIT");
            return CABOR_IR_VAR_INVALID;
        }
    }
}

cabor_ir_var_idx cabor_visit_ir_identifier(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    cabor_token* token = TOKEN(root_expr);
    bool found = false;
    cabor_ir_var_idx var_idx = cabor_map_get(root_tab->map, token->data, &found);

    if (!found)
    {
        CABOR_LOG_ERR_F("IR error: visit_ir_identifier didn't find ir var for %s", token->data);
        return CABOR_IR_VAR_INVALID;
    }

    return var_idx;
}

cabor_ir_var_idx cabor_visit_ir_function_call(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    cabor_token* token = TOKEN(root_expr);
    bool found = false;
    cabor_ir_var_idx fun_idx = cabor_map_get(root_tab->map, token->data, &found);

    int num_args = root_expr->num_edges;
    cabor_ir_var_idx args[16]; // max 16 args for now
    for (int i = 0; i < num_args; i++)
    {
        args[i] = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[i]), root_tab);
    }

    cabor_ir_var_idx dest = cabor_create_unique_ir_var(ir_data, root_expr->type);
    cabor_create_ir_call(ir_data, fun_idx, args, num_args, dest);
    return dest;
}

cabor_ir_var_idx cabor_visit_ir_unit(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    return CABOR_IR_VAR_UNIT;
}

cabor_ir_var_idx cabor_visit_ir_block(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    cabor_ir_var_idx result = CABOR_IR_VAR_UNIT;

    cabor_symbol_table* new_scope = cabor_create_new_symbol_scope(root_tab);

    for (int i = 0; i < root_expr->num_edges; i++)
    {
        result = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[i]), new_scope);
    }

    return result;
}

cabor_ir_var_idx cabor_visit_ir_if_then_else(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    if (root_expr->num_edges == 2)
    {
        cabor_ir_label_idx l_then = cabor_create_ir_label(ir_data, "then");
        cabor_ir_label_idx l_end = cabor_create_ir_label(ir_data, "end");

        cabor_ir_var_idx var_cond = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[0]), root_tab);
        cabor_ir_inst_idx cond_jump = cabor_create_ir_condjump(ir_data, var_cond, l_then, l_end);

        cabor_push_ir_label(ir_data, l_then);
        cabor_ir_var_idx var_then = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[1]), root_tab);
        cabor_ir_inst_idx jump_end = cabor_create_ir_jump(ir_data, l_end);

        cabor_push_ir_label(ir_data, l_then);

        return CABOR_IR_VAR_UNIT;
    }
    else
    {
        cabor_ir_label_idx l_then = cabor_create_ir_label(ir_data, "then");
        cabor_ir_label_idx l_end = cabor_create_ir_label(ir_data, "end");
        cabor_ir_label_idx l_else = cabor_create_ir_label(ir_data, "else");

        cabor_ir_var_idx var_cond = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[0]), root_tab);
        cabor_ir_inst_idx cond_jump = cabor_create_ir_condjump(ir_data, var_cond, l_then, l_else);
        cabor_push_ir_label(ir_data, l_then);

        cabor_ir_var_idx var_then = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[1]), root_tab);
        cabor_ir_var_idx var_result = cabor_create_unique_ir_var(ir_data, IR_VAR_IDX(ir_data, var_then)->type);
        cabor_ir_var_idx copy1 = cabor_create_ir_copy(ir_data, var_then, var_result);

        cabor_ir_inst_idx jump_to_end_from_then = cabor_create_ir_jump(ir_data, l_end);

        cabor_push_ir_label(ir_data, l_else);
        cabor_ir_var_idx var_else = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[2]), root_tab);
        cabor_ir_var_idx copy2 = cabor_create_ir_copy(ir_data, var_else, var_result);
        cabor_ir_inst_idx jump_to_end_from_else = cabor_create_ir_jump(ir_data, l_end);

        cabor_push_ir_label(ir_data, l_end);

        return var_result;
    }
}

cabor_ir_var_idx cabor_visit_ir_while(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    cabor_ir_label_idx l_start = cabor_create_ir_label(ir_data, "while_start");
    cabor_ir_label_idx l_body = cabor_create_ir_label(ir_data, "while_body");
    cabor_ir_label_idx l_end = cabor_create_ir_label(ir_data, "while_end");

    cabor_push_ir_label(ir_data, l_start);
    cabor_ir_var_idx cond = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[0]), root_tab);
    cabor_create_ir_condjump(ir_data, cond, l_body, l_end);

    cabor_push_ir_label(ir_data, l_body);
    cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[1]), root_tab);
    cabor_create_ir_jump(ir_data, l_start);

    cabor_push_ir_label(ir_data, l_end);

    return CABOR_IR_VAR_UNIT;
}

cabor_ir_var_idx cabor_visit_ir_var_expr(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    cabor_token* token = TOKEN(ROOT(&root_expr->edges[0]));
    cabor_type type = root_expr->type;
    cabor_ir_var_idx var = cabor_create_ir_var(ir_data, token->data, type);

    cabor_ir_var_idx value = cabor_visit_ir_node(ir_data, ast, ROOT(&root_expr->edges[1]), root_tab);
    cabor_create_ir_copy(ir_data, value, var);
    cabor_map_insert(root_tab->map, token->data, var);

    return CABOR_IR_VAR_UNIT;
}

cabor_ir_var_idx cabor_visit_ir_declaration(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab)
{
    // no reason to do anything here
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
