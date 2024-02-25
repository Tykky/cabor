#include "test_framework.h"
#include "../logging/logging.h"

#include <stddef.h>
#include <string.h>

#ifdef CABOR_ENABLE_TESTING

static cabor_test_framework_context test_context;

void initialize_cabor_test_framework(cabor_test_framework_context* ctx)
{
	memset(ctx->tests, 0, sizeof(cabor_test_case) * CABOR_TEST_FRAMEWORK_MAX_TESTS);
	ctx->current_size = 0;
}

void add_cabor_test(cabor_test_framework_context* ctx, cabor_test_case* test)
{
	ctx->tests[ctx->current_size++] = *test;
}

void run_cabor_tests(cabor_test_framework_context* ctx)
{
	unsigned int passed = 0;
	CABOR_LOG_TEST("runig cabor tests...");
	for (size_t i = 0; i < ctx->current_size; i++)
	{
		cabor_test_case* test = &ctx->tests[i];
		int result = test->test_func();
		if (!result)
		{
			CABOR_LOG_TEST_F("-- (%d/%d) cabor test %s success!", i + 1, ctx->current_size, test->name);
			passed++;
		}
		else
		{
			CABOR_LOG_TEST_F("-- (%d/%d) cabor test %s failed!", i + 1, ctx->current_size, test->name);
		}
	}
	CABOR_LOG_TEST_F("Cabor finsihed running tests! %d/%d passed!", passed, ctx->current_size);
}

cabor_test_framework_context* get_global_cabor_test_framework_context()
{
	return &test_context;
}

#endif
