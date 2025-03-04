#pragma once

#include "../../cabor_defines.h"

#ifdef CABOR_ENABLE_TESTING

#include "../test_framework.h"
#include "../../language/parser.h"

// Unit tests
int cabor_test_parse_expression_abc();
int cabor_test_parse_expression_cba();
int cabor_test_parse_expression_abc_parenthesized();
int cabor_test_parse_expression_if_then_else();
int cabor_test_parse_expression_if_then();
int cabor_test_parse_function_hello();

// Integration tokenizer + parser
int cabor_integration_test_parse_expression_abc();
int cabor_integration_test_parse_expression_cba();
int cabor_integration_test_parse_expression_abc_parenthesized();
int cabor_integration_test_parse_expression_if_then_else();
int cabor_integration_test_parse_expression_if_then();
int cabor_integration_test_parse_function_hello1();
int cabor_integration_test_parse_function_hello2();
int cabor_integration_test_parse_function_hello3();
int cabor_integration_test_parse_function_hello4();
int cabor_integration_test_parse_expression_additional1();
int cabor_integration_test_parse_expression_additional2();
int cabor_integration_test_parse_expression_additional3();
int cabor_integration_test_parse_expression_additional4();
int cabor_integration_test_parse_expression_additional5();
int cabor_integration_test_parse_expression_additional6();
int cabor_integration_test_parse_expression_additional7();
int cabor_integration_test_parse_any_nested_function_calls();
int cabor_integration_test_parse_unary_inside_function_call();
int cabor_integration_test_parse_nested_if_statements();
int cabor_integration_test_parse_nested_if_with_function_calls();
int cabor_integration_test_parse_binary_op_with_function_call_operands();
int cabor_integration_test_parse_while_foo_do_bar();
int cabor_integration_test_parse_variable_assignemnt();
int cabor_integration_test_parse_block_expression();
int cabor_integration_test_parse_block_expression_ending_none();


#endif // CABOR_ENABLE_TESTING

