#pragma once

#include "../core/vector.h"
#include "../cabor_defines.h"
#include "../core/hashmap.h"
#include "../language/type_checker.h"
#include <stdint.h>

#define CABOR_MAX_IR_VAR_LENGTH 64
#define CABOR_MAX_LABEL_LENGTH 64

typedef int cabor_ir_var_idx;
typedef int cabor_ir_label_idx;
typedef int cabor_ir_inst_idx;
typedef int cabor_ir_inst_idx;
typedef cabor_map_entry cabor_ir_var_entry;

typedef struct cabor_ir_var_t
{
    char name[CABOR_MAX_IR_VAR_LENGTH];
    cabor_ir_var_idx id; // ir vars are unique
    cabor_type type;
} cabor_ir_var;

typedef struct
{
    cabor_hash_map*      ir_var_types;     // maps all global names like 'print_int', '+' to to their types
    cabor_symbol_table*  ir_symtab;        // maps all global names to unique ir_var
    cabor_vector*        ir_vars;          // all cabor_ir_var objects
    cabor_vector*        ir_labels;        // all cabor_ir_label objects
    cabor_vector*        ir_instructions;  // all cabor_ir_instruction objects
    cabor_vector*        ir_call_args;     // storage for all function call argument lists stored just after each other in memory:
} cabor_ir_data;                           // [1, 4, 7 ,3], [2, 3, 8] <- indices to ir_vars array
                                           //   call1        call2    ...
typedef enum
{
    CABOR_IR_INST_LOAD_BOOL,
    CABOR_IR_INST_LOAD_INT,
    CABOR_IR_INST_COPY,
    CABOR_IR_INST_CALL,
    CABOR_IR_INST_JUMP,
    CABOR_IR_INST_CONDJUMP,
    CABOR_IR_INST_LABEL,
    CABOR_IR_INST_UNKNOWN
} cabor_ir_instruction_type;

// We separate cabor_ir_label and cabor_ir_label_instruction so
// labels can be decalred without inserting them into instruction list

typedef struct cabor_ir_label_t
{
    char name[CABOR_MAX_LABEL_LENGTH];
} cabor_ir_label;

typedef struct
{
    cabor_ir_label_idx idx;
} cabor_ir_label_instruction;

typedef struct
{
    bool value;
    cabor_ir_var_idx dest;
} cabor_ir_load_bool_const;

typedef struct
{
    int value;
    cabor_ir_var_idx dest;
} cabor_ir_load_int_const;

typedef struct
{
    cabor_ir_var_idx source;
    cabor_ir_var_idx dest;
} cabor_ir_copy;

typedef struct
{
    cabor_ir_var_idx fun;
    cabor_ir_var_idx* args;
    int num_args;
    cabor_ir_var_idx dest;
} cabor_ir_call;

typedef struct
{
    cabor_ir_label_idx label;
} cabor_ir_jump;

typedef struct
{
    cabor_ir_var_idx cond;
    cabor_ir_label_idx then_label;
    cabor_ir_label_idx else_label;
} cabor_ir_condjump;

typedef struct cabor_ir_instruction_t
{
    cabor_ir_instruction_type type;
    union
    {
        cabor_ir_load_bool_const load_bool_const;
        cabor_ir_load_int_const load_int_const;
        cabor_ir_copy copy;
        cabor_ir_call call;
        cabor_ir_jump jump;
        cabor_ir_label_instruction label;
        cabor_ir_condjump cond_jump;
    };
} cabor_ir_instruction;


size_t cabor_get_ir_instruction_size();
size_t cabor_get_ir_var_size();
size_t cabor_get_ir_label_size();

// These allocate and destroy all the data required for IR generation
cabor_ir_data* cabor_create_ir_data();
void cabor_destroy_ir_data(cabor_ir_var* ir_var_types);

// No need to bother with deallocating individual ir instructions, cabor_destroy_ir_data handles that
cabor_ir_var_idx cabor_create_ir_var(cabor_ir_data* ir_data, const char* var, cabor_type type);
cabor_map_entry* cabor_create_ir_var_with_entry(cabor_ir_data* ir_data, const char* var, cabor_type type, cabor_symbol_table* symtab);
cabor_ir_var_idx cabor_create_unique_ir_var(cabor_ir_data* ir_data, cabor_type type);
cabor_ir_label_idx cabor_create_ir_label(cabor_ir_data* ir_data, const char* label);
cabor_ir_inst_idx cabor_push_ir_label(cabor_ir_data* ir_data, cabor_ir_label_idx label);
cabor_ir_inst_idx cabor_create_ir_load_bool_const(cabor_ir_data* ir_data, bool value, int dest);
cabor_ir_inst_idx cabor_create_ir_load_int_const(cabor_ir_data* ir_data, int value, int dest);
cabor_ir_inst_idx cabor_create_ir_copy(cabor_ir_data* ir_data, int source, int dest);
cabor_ir_inst_idx cabor_create_ir_call(cabor_ir_data* ir_data, int fun, int* args, int num_args, int dest);
cabor_ir_inst_idx cabor_create_ir_jump(cabor_ir_data* ir_data, int label);
cabor_ir_inst_idx cabor_create_ir_condjump(cabor_ir_data* ir_data, int cond, int then_label, int else_label);

// Get ir var from scoped sym tab
cabor_ir_var_entry* cabor_get_ir_var_entry(cabor_symbol_table* sym_tab, const char* ir_var);

void cabor_generate_ir(cabor_ir_data* var_types, cabor_ast* ast, cabor_ast_node* root_expr);

void cabor_format_ir_instruction(cabor_ir_data* ir_data, cabor_ir_inst_idx inst, char* buffer, size_t bufSize);

cabor_map_entry* cabor_require_ir_var(cabor_ir_data* ir_data, cabor_symbol_table* symtab, const char* var, cabor_type type);

cabor_ir_var_idx cabor_visit_ir_binaryop(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_unaryop(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_literal(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_identifier(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_function_call(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_unit(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_block(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_if_then_else(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_while(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_var_expr(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
cabor_ir_var_idx cabor_visit_ir_declaration(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);

// Returns index to IR_VAR in ir_data->ir_vars
int cabor_visit_ir_node(cabor_ir_data* ir_data, cabor_ast* ast, cabor_ast_node* root_expr, cabor_symbol_table* root_tab);
