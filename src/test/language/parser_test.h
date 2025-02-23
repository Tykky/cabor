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


#endif // CABOR_ENABLE_TESTING

