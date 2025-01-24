#pragma once

#include "../cabor_defines.h"
#include "../logging/logging.h"
#include "../debug/cabor_debug.h"
#include <stddef.h>

#ifdef CABOR_ENABLE_TESTING

#define CABOR_TEST_FRAMEWORK_MAX_TESTS 4096
#define CABOR_INITIALIZE_TEST_FRAMEWORK() initialize_cabor_test_framework(get_global_cabor_test_framework_context()) 
#define CABOR_REGISTER_TEST(test_name, function) cabor_test_case test_case_##function = { .name = test_name, .test_func = function}; add_cabor_test(get_global_cabor_test_framework_context(), &test_case_##function)
#define CABOR_RUN_ALL_TESTS() run_cabor_tests(get_global_cabor_test_framework_context())

#define CABOR_CHECK_EQUALS(a, b, res) if (!(a == b)) { res = 1; CABOR_LOG_TEST_F("-- CHECK_EQUALS failed, %d != %d", a, b); CABOR_TEST_ERROR("CHECK_EQUALS failed"); }
#define CABOR_CHECK_GREATER(a, b, res) if (!(a > b)) { res = 1; CABOR_LOG_TEST_F("-- CHECK_GREATER failed, %d > %d", a, b); CABOR_TEST_ERROR("CHECK_GREATER failed"); }
#define CABOR_CHECK_GREATER_EQ(a, b, res) if (!(a >= b)) { res = 1; CABOR_LOG_TEST_F("-- CHECK_GREATER_EQ failed, %d >= %d", a, b); CABOR_TEST_ERROR("CHECK_GREATER_EQ failed"); }
#define CABOR_CHECK_LESS(a, b, res) if (!(a < b)) { *res = 1; CABOR_LOG_TEST_F("-- CHECK_LESS failed, %d < %d", a, b); CABOR_TEST_ERROR("CHECK_LESS failed"); }
#define CABOR_CHECK_LESS_EQ(a, b, res) if (!(a <= b)) { res = 1; CABOR_LOG_TEST_F("-- CHECK_LESS_EQ failed, %d < %d", a, b); CABOR_TEST_ERROR("CHECK_LESS_EQ failed"); }

typedef struct
{
    const char* name;
    int (*test_func)(void);
} cabor_test_case;

typedef struct
{
    cabor_test_case tests[CABOR_TEST_FRAMEWORK_MAX_TESTS];
    size_t current_size;
} cabor_test_framework_context;

void initialize_cabor_test_framework(cabor_test_framework_context* ctx);
void add_cabor_test(cabor_test_framework_context* ctx, cabor_test_case* test);
int run_cabor_tests(cabor_test_framework_context* ctx);

cabor_test_framework_context* get_global_cabor_test_framework_context();

#else

static inline int cabor_do_nothing() { return 0; }

#define CABOR_INITIALIZE_TEST_FRAMEWORK()
#define CABOR_REGISTER_TEST(name, function)
#define CABOR_RUN_ALL_TESTS() cabor_do_nothing()

#endif
