#include "test_framework.h"
#include "../logging/logging.h"
#include "../core/cabortime.h"

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

int run_cabor_tests(cabor_test_framework_context* ctx)
{
	unsigned int passed = 0;
	unsigned int failed = 0;
	CABOR_LOG_TEST_F("%-60s | %-10s | %-8s | %-7s |", "Test description", "Status", "Time(ns)", "Count");
	CABOR_LOG_TEST("------------------------------------------------------------------------------------------------");
	for (size_t i = 0; i < ctx->current_size; i++)
	{
		cabor_test_case* test = &ctx->tests[i];
		double start = cabor_get_time();
		int result = test->test_func();
		double end = cabor_get_time();
		double diff_ns = (end - start) * 1000000.f;
		if (!result)
		{
			passed++;
		}
		else
		{
			failed++;
		}
        CABOR_LOG_TEST_F("%-60s | %-10s | %-8.1f | %.3d/%.3d |", test->name, !result ? "Success" : "Failed", diff_ns, i + 1, ctx->current_size);
	}
	CABOR_LOG_TEST("------------------------------------------------------------------------------------------------");
	if (failed)
		CABOR_LOG_TEST_F("Cabor failed %d/%d of tests!", failed, ctx->current_size);
	else
		CABOR_LOG_TEST_F("Succeeded in all of %d/%d tests!", passed, ctx->current_size);
	return failed > 0 ? 1 : 0;
}

cabor_test_framework_context* get_global_cabor_test_framework_context()
{
	return &test_context;
}

#endif
