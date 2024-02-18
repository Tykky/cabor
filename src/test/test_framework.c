#include "test_framework.h"
#include "stddef.h"

void run_tests(test_framework_context* ctx)
{
	for (size_t i = 0; i < CABOR_TEST_FRAMEWORK_MAX_TESTS; i++)
	{
		test_case* test = &ctx->tests[i];
	}
}
