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
    CABOR_REGISTER_TEST("test_vector_push", cabor_test_vector_push);
    CABOR_REGISTER_TEST("test_vector_get", cabor_test_vector_get);
    CABOR_REGISTER_TEST("test_vector_peek", cabor_test_vector_peek);
    CABOR_REGISTER_TEST("test_vector_resize", cabor_test_vector_resize);
    CABOR_REGISTER_TEST("test_vector_zero_initialized", cabor_test_zero_vector_initialized);

    // Stack tests
    CABOR_REGISTER_TEST("test_stack_push", cabor_test_stack_push);
    CABOR_REGISTER_TEST("test_stack_pop", cabor_test_stack_pop);
    CABOR_REGISTER_TEST("test_stack_peek", cabor_test_stack_peek);

    // Filesystem tests
    CABOR_REGISTER_TEST("test_load_file", cabor_test_load_file);

    // Language tests
    CABOR_REGISTER_TEST("test_tokenize_hello_world", cabor_test_tokenize_hello_world);
    CABOR_REGISTER_TEST("test_parse_expression_abc", cabor_test_parse_expression_abc);
    CABOR_REGISTER_TEST("test_parse_expression_cba", cabor_test_parse_expression_cba);
    CABOR_REGISTER_TEST("test_parse_expression_abc_parenthesized", cabor_test_parse_expression_abc_parenthesized);
}

#else
void register_all_tests() {}
#endif
