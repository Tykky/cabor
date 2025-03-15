#pragma once

#include "../../cabor_defines.h"

#ifdef CABOR_ENABLE_TESTING

#include "../test_framework.h"
#include "../../language/parser.h"

int cabor_integration_test_ir_basic_expression();
int cabor_integration_test_ir_if_then_else();
int cabor_integration_test_ir_if_then();
int cabor_integration_test_ir_var_expr();
int cabor_integration_test_ir_var_expr2();
int cabor_integration_test_ir_unary_op();
int cabor_integration_test_ir_while();


#endif

