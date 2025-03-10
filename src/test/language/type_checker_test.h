#pragma once

#include "../../cabor_defines.h"

#ifdef CABOR_ENABLE_TESTING

#include "../test_framework.h"
#include "../../language/parser.h"

int cabor_integration_test_typecheck_var_declaration();
int cabor_integration_test_typecheck_var_declaration2();
int cabor_integration_test_typecheck_if_then_else();
int cabor_integration_test_typecheck_unary_op_not();
int cabor_integration_test_typecheck_unary_op_minus();
int cabor_integration_test_typecheck_while_loop();
int cabor_integration_test_typecheck_undeclared_identifier();
int cabor_integration_test_typecheck_mismatched_binary_op();
int cabor_integration_test_typecheck_mismatched_variable_decl();
int cabor_integration_test_typecheck_duplicate_decl_same_scope();
int cabor_integration_test_typecheck_mismatched_branch_types();
int cabor_integration_test_typecheck_not_bool_if();
int cabor_integration_test_typecheck_not_bool_while();
int cabor_integration_test_typecheck_scoping_rules();


#endif

