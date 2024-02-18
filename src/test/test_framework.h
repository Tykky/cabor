#pragma once

#define CABOR_TEST_FRAMEWORK_MAX_TESTS 4096

typedef struct
{
    const char* name;
    int (*test_func)(void);
} test_case;

typedef struct
{
    // just do this so we can avoid creating dependency
    // to cabor_vector
    test_case tests[CABOR_TEST_FRAMEWORK_MAX_TESTS];
} test_framework_context;

void run_tests(test_framework_context* ctx);
