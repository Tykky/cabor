#pragma once

#include "../language/parser.h"
#include "../core/vector.h"
#include "../cabor_defines.h"
#include "../core/hashmap.h"
#include <stdint.h>


typedef struct cabor_symbol_table_t
{
    cabor_hash_map* map; // maps c string -> int (i.e cabor_type)
    struct cabor_symbol_table_t* parent_scope;
    struct cabor_symbol_table_t* child_scope;
} cabor_symbol_table;

cabor_symbol_table* cabor_create_symbol_table();
void cabor_destroy_symbol_table(cabor_symbol_table* symbol_table);

cabor_symbol_table* cabor_create_new_symbol_scope(cabor_symbol_table* symbol_table);

cabor_type cabor_convert_type_declaration_to_type(cabor_token* type_decl);
cabor_type cabor_typecheck_if_then_else(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table);
cabor_type cabor_typecheck_binary_op(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table);
cabor_type cabor_typecheck(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table);
cabor_type cabor_typecheck_unary_op(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table);
cabor_type cabor_typecheck_function(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table);
cabor_type cabor_typecheck_while(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table);
cabor_type cabor_typecheck_block(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table);
cabor_type cabor_typecheck_var_expr(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* sym_table);
cabor_type cabor_typecheck_literal(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* symb_table);
cabor_type cabor_typecheck_identifier(cabor_ast* ast, cabor_ast_node* node, cabor_symbol_table* symb_table);
