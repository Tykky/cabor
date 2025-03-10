#include "registered_tests.h"

#ifdef CABOR_ENABLE_TESTING

#include "core/vector_test.h"
#include "core/stack_test.h"
#include "core/hashmap_test.h"
#include "filesystem/filesystem_tests.h"
#include "language/tokenizer_test.h"
#include "language/parser_test.h"
#include "language/type_checker_test.h"

void register_all_tests()
{
    // Vector tests
    CABOR_REGISTER_TEST("UNIT vector push", cabor_test_vector_push);
    CABOR_REGISTER_TEST("UNIT vector get", cabor_test_vector_get);
    CABOR_REGISTER_TEST("UNIT vector peek", cabor_test_vector_peek);
    CABOR_REGISTER_TEST("UNIT vector resize", cabor_test_vector_resize);
    CABOR_REGISTER_TEST("UNIT vector zero initialized", cabor_test_zero_vector_initialized);

    // Hashmap tests
    CABOR_REGISTER_TEST("UNIT hashmap insert and get", cabor_unit_test_hashmap_insert_and_get);
    CABOR_REGISTER_TEST("UNIT hashmap collisions", cabor_unit_test_hashmap_collison_test);
    CABOR_REGISTER_TEST("UNIT hashmap tiny collisions", cabor_unit_test_hashmap_tiny_collisions);

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
    CABOR_REGISTER_TEST("INTEGRATION parse function hello1()", cabor_integration_test_parse_function_hello1);
    CABOR_REGISTER_TEST("INTEGRATION parse function hello2()", cabor_integration_test_parse_function_hello2);
    CABOR_REGISTER_TEST("INTEGRATION parse function hello3()", cabor_integration_test_parse_function_hello3);
    CABOR_REGISTER_TEST("INTEGRATION parse function hello4()", cabor_integration_test_parse_function_hello4);
    CABOR_REGISTER_TEST("INTEGRATION parse expression additional 1", cabor_integration_test_parse_expression_additional1);
    CABOR_REGISTER_TEST("INTEGRATION parse expression additional 2", cabor_integration_test_parse_expression_additional2);
    CABOR_REGISTER_TEST("INTEGRATION parse expression additional 3", cabor_integration_test_parse_expression_additional3);
    CABOR_REGISTER_TEST("INTEGRATION parse expression additional 4", cabor_integration_test_parse_expression_additional4);
    CABOR_REGISTER_TEST("INTEGRATION parse expression additional 5", cabor_integration_test_parse_expression_additional5);
    CABOR_REGISTER_TEST("INTEGRATION parse expression with additional 6", cabor_integration_test_parse_expression_additional6);
    CABOR_REGISTER_TEST("INTEGRATION parse expression with additioanl 7", cabor_integration_test_parse_expression_additional7);
    CABOR_REGISTER_TEST("INTEGRATION parse nested function calls", cabor_integration_test_parse_any_nested_function_calls);
    CABOR_REGISTER_TEST("INTEGRATION parse unary inside function call", cabor_integration_test_parse_unary_inside_function_call);
    CABOR_REGISTER_TEST("INTEGRATION parse nested if statements", cabor_integration_test_parse_nested_if_statements);
    CABOR_REGISTER_TEST("INTEGRATION parse nested if with function calls", cabor_integration_test_parse_nested_if_with_function_calls);
    CABOR_REGISTER_TEST("INTEGRATION parse binary op with function call operands", cabor_integration_test_parse_binary_op_with_function_call_operands);
    CABOR_REGISTER_TEST("INTEGRATION parse while foo do bar", cabor_integration_test_parse_while_foo_do_bar);
    CABOR_REGISTER_TEST("INTEGRATION parse variable assignment", cabor_integration_test_parse_variable_assignemnt);
    CABOR_REGISTER_TEST("INTEGRATION parse block expression", cabor_integration_test_parse_block_expression);
    CABOR_REGISTER_TEST("INTEGRATION parse block expression ending none", cabor_integration_test_parse_block_expression_ending_none);
    CABOR_REGISTER_TEST("INTEGRATION parse block complex expression", cabor_integration_test_parse_block_complex_expression);
    CABOR_REGISTER_TEST("INTEGRATION parse variable assignment with type decl", cabor_integration_test_parse_variable_assignment_with_type_declaration);
   
    // Typechecker tests
    CABOR_REGISTER_TEST("INTEGRATION typecheck var declaration", cabor_integration_test_typecheck_var_declaration);
    CABOR_REGISTER_TEST("INTEGRATION typecheck var declaration 2", cabor_integration_test_typecheck_var_declaration2);
    CABOR_REGISTER_TEST("INTEGRATION typecheck if then else", cabor_integration_test_typecheck_if_then_else);
    CABOR_REGISTER_TEST("INTEGRATION typecheck unary op not", cabor_integration_test_typecheck_unary_op_not);
    CABOR_REGISTER_TEST("INTEGRATION typecheck unary op minus", cabor_integration_test_typecheck_unary_op_minus);
    CABOR_REGISTER_TEST("INTEGRATION typecheck while loop", cabor_integration_test_typecheck_while_loop);
    CABOR_REGISTER_TEST("INTEGRATION typecheck undeclared identifier", cabor_integration_test_typecheck_undeclared_identifier);
    CABOR_REGISTER_TEST("INTEGRATION typecheck mismatched binary op", cabor_integration_test_typecheck_mismatched_binary_op);
    CABOR_REGISTER_TEST("INTEGRATION typecheck mismatched variable declaration", cabor_integration_test_typecheck_mismatched_variable_decl);
    CABOR_REGISTER_TEST("INTEGRATION typecheck duplicate declaration same scope", cabor_integration_test_typecheck_duplicate_decl_same_scope);
    CABOR_REGISTER_TEST("INTEGRATION typecheck mismatched branch types", cabor_integration_test_typecheck_mismatched_branch_types);
    CABOR_REGISTER_TEST("INTEGRATION typecheck not bool if", cabor_integration_test_typecheck_not_bool_if);
    CABOR_REGISTER_TEST("INTEGRATION typecheck not bool while", cabor_integration_test_typecheck_not_bool_while);
    CABOR_REGISTER_TEST("INTEGRATION typecheck scoping rules", cabor_integration_test_typecheck_scoping_rules);
}
#else
void register_all_tests() {}
#endif
