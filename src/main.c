#include "cabor_defines.h"

#include <stdio.h>
#include <string.h>

#include "core/vector.h"
#include "core/memory.h"
#include "filesystem/filesystem.h"
#include "language/tokenizer.h"
#include "language/parser.h"

#include "logging/logging.h"

#include "test/test_framework.h"
#include "test/registered_tests.h" 

#include <jansson.h>

#define CABOR_ARG_ENABLE_TESTING (1 << 0)
#define CABOR_ARG_TOKENIZE (1 << 1)
#define CABOR_ARG_PARSE (1 << 2)

static unsigned int parse_cmd_args(int argc, char** argv, int* tokenize_arg, int* parse_arg)
{
	if (argc < 2)
		return 0;

	unsigned int bit_flags = 0;

	for (int i = 1; i < argc; i++)
	{
		char* arg = argv[i];
		if (!strcmp(arg, "--test") || !strcmp(arg, "-te"))
		{
			bit_flags |= CABOR_ARG_ENABLE_TESTING;
		}

		if (!strcmp(arg, "--tokenize") || !strcmp(arg, "-to"))
		{
			bit_flags |= CABOR_ARG_TOKENIZE;
			*tokenize_arg = i + 1;
		}

		if (!strcmp(arg, "--parse") || !strcmp(arg, "-p"))
		{
			bit_flags |= CABOR_ARG_PARSE;
			*parse_arg = i + 1;
		}
	}

	return bit_flags;
}

static void run_tokenizer(const char* filename)
{
	cabor_file file = cabor_load_file(filename);
	cabor_vector tokens = cabor_tokenize(&file);

	size_t buffer_size = tokens.size * sizeof(cabor_token);
	cabor_allocation buffer = CABOR_MALLOC(buffer_size);
	cabor_stringify_tokens(buffer.mem, buffer_size, &tokens);
	CABOR_LOG_F("%s", buffer);

	cabor_destroy_file(&file);
	destroy_cabor_vector(&tokens);

	CABOR_FREE(&buffer);
}

static void run_parser(const char* filename)
{
}

int main(int argc, char **argv) 
{
	CABOR_CREATE_ALLOCATOR();
	CABOR_INITIALIZE_TEST_FRAMEWORK();
	CABOR_CREATE_LOGGER();

	int tokenize_arg;
	int parse_arg;

	unsigned int flags = parse_cmd_args(argc, argv, &tokenize_arg, &parse_arg);
	unsigned int test_results = 0;

	if (flags & CABOR_ARG_ENABLE_TESTING)
	{
		register_all_tests();
		test_results = CABOR_RUN_ALL_TESTS();
	}

	if (flags & CABOR_ARG_TOKENIZE)
	{
		run_tokenizer(argv[tokenize_arg]);
	}

	if (flags & CABOR_ARG_PARSE)
	{
		run_parser(argv[tokenize_arg]);
	}

	CABOR_DUMP_LOG_TO_DISK();
	CABOR_DESTROY_LOGGER();

#if CABOR_ENABLE_MEMORY_DEBUGGING 

	size_t current_allocated = CABOR_GET_ALLOCATED();
	cabor_allocator_context* allocator = CABOR_GET_ALLOCATOR();

	// check for memory leaks
	if (current_allocated > 0)
	{ 
		double size;
		const char* prefix = cabor_convert_bytes_to_human_readable(current_allocated, &size);
        printf("\nLeak detected!, there is %.2f %s of unfreed memory!\n", size, prefix);
#if CABOR_ENABLE_MEMORY_DEBUGGING 
		printf("\n------------PRINTING ALL ALLOCATOR CALL LOCATIONS------------------\n\n");
		for (size_t i = 0; i < allocator->debug_size; i++)
		{
			printf("%s", allocator->debug[i]);
		}
		printf("\n------------------END PRINTING ALLOCATOR CALLS---------------------\n");
		printf("\n------------PRINTING ALL DEALLOCATOR CALL LOCATIONS----------------\n\n");
		for (size_t i = 0; i < allocator->dealloc_size; i++)
		{
			printf("%s", allocator->dealloc[i]);
		}
		printf("\n----------------END PRINTING DEALLOCATOR CALLS---------------------\n");
#endif

		return 1;
	}
	else
	{
		printf("No leak detected!\n");
	}

#endif

	CABOR_DESTROY_ALLOCATOR();

	return test_results;
}