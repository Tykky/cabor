#include "registered_tests.h"

#ifdef CABOR_ENABLE_TESTING

#include "core/vector_test.h"
#include "core/stack_test.h"
#include "filesystem/filesystem_tests.h"
#include "language/tokenizer_test.h"
#include "language/parser_test.h"

void register_all_tests()
{
    // Vector tests
    CABOR_REGISTER_TEST("UNIT vector push", cabor_test_vector_push);
    CABOR_REGISTER_TEST("UNIT vector get", cabor_test_vector_get);
    CABOR_REGISTER_TEST("UNIT vector peek", cabor_test_vector_peek);
    CABOR_REGISTER_TEST("UNIT vector resize", cabor_test_vector_resize);
    CABOR_REGISTER_TEST("UNIT vector zero initialized", cabor_test_zero_vector_initialized);

    // Stack tests
    CABOR_REGISTER_TEST("UNIT stack push", cabor_test_stack_push);
    CABOR_REGISTER_TEST("UNIT stack pop", cabor_test_stack_pop);
    CABOR_REGISTER_TEST("UNIT stack peek", cabor_test_stack_peek);

    // Filesystem tests
    CABOR_REGISTER_TEST("INTEGRATION load file", cabor_test_load_file);

    // Language tests
    CABOR_REGISTER_TEST("UNIT tokenize hello world", cabor_test_tokenize_hello_world);
    CABOR_REGISTER_TEST("UNIT parse expression abc", cabor_test_parse_expression_abc);
    CABOR_REGISTER_TEST("UNIT parse expression cba", cabor_test_parse_expression_cba);
    CABOR_REGISTER_TEST("UNIT parse expression abc parenthesized", cabor_test_parse_expression_abc_parenthesized);
    CABOR_REGISTER_TEST("UNIT parse expression if then else", cabor_test_parse_expression_if_then_else);
    CABOR_REGISTER_TEST("UNIT parse expression if then", cabor_test_parse_expression_if_then);
    CABOR_REGISTER_TEST("UNIT parse function hello()", cabor_test_parse_function_hello);

    CABOR_REGISTER_TEST("INTEGRATION parse expression abc", cabor_integration_test_parse_expression_abc);
    CABOR_REGISTER_TEST("INTEGRATION parse expression cba", cabor_integration_test_parse_expression_cba);
    CABOR_REGISTER_TEST("INTEGRATION parse expression abc parenthesized", cabor_integration_test_parse_expression_abc_parenthesized);
    CABOR_REGISTER_TEST("INTEGRATION parse expression if then else", cabor_integration_test_parse_expression_if_then_else);
    CABOR_REGISTER_TEST("INTEGRATION parse expression if then", cabor_integration_test_parse_expression_if_then);
}

#else
void register_all_tests() {}
#endif
