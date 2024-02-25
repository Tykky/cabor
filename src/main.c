#include "cabor_defines.h"

#include <stdio.h>
#include <string.h>

#include "core/vector.h"
#include "core/memory.h"
#include "logging/logging.h"

#include "test/test_framework.h"
#include "test/registered_tests.h" 

#define CABOR_ARG_ENABLE_TESTING (1 << 0)

static unsigned int parse_cmd_args(int argc, char** argv)
{
	if (argc < 2)
		return 0;

	unsigned int bit_flags = 0;

	for (int i = 1; i < argc; i++)
	{
		char* arg = argv[i];
		if (!strcmp(arg, "--test"))
			bit_flags |= CABOR_ARG_ENABLE_TESTING;
	}

	return bit_flags;
}

int main(int argc, char **argv) 
{
	CABOR_CREATE_ALLOCATOR();
	CABOR_INITIALIZE_TEST_FRAMEWORK();
	CABOR_CREATE_LOGGER();

	unsigned int flags = parse_cmd_args(argc, argv);

	if (flags & CABOR_ARG_ENABLE_TESTING)
	{
		register_all_tests();
		CABOR_RUN_ALL_TESTS();
	}

	CABOR_DUMP_LOG_TO_DISK();
	CABOR_DESTROY_LOGGER();

	size_t current_allocated = CABOR_GET_ALLOCATED();

	// check for memory leaks
	if (current_allocated > 0)
	{ 
		double size;
		const char* prefix = cabor_convert_bytes_to_human_readable(current_allocated, &size);
        printf("Leak detected!, there %.2f %s of unfreed memory!", size, prefix);
		return 1;
	}
	else
	{
		printf("No leak detected!");
	}

	CABOR_DESTROY_ALLOCATOR();

	return 0;
}