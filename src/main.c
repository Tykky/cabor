#include "cabor_defines.h"

#ifdef _DEBUG && WIN32
#include <stdlib.h>
#include <crtdbg.h>k
#define malloc(s)       _malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define calloc(c, s)    _calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#define realloc(p, s)   _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include <stdio.h>
#include <string.h>

#include "core/vector.h"
#include "core/memory.h"
#include "filesystem/filesystem.h"
#include "language/tokenizer.h"
#include "language/parser.h"
#include "language/compiler.h"

#include "logging/logging.h"

#include "network/network.h"

#include "test/test_framework.h"
#include "test/registered_tests.h" 

#include "core/cabortime.h"

#ifdef _DEBUG 
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif


#define CABOR_ARG_ENABLE_TESTING (1 << 0)
#define CABOR_ARG_TOKENIZE (1 << 1)
#define CABOR_ARG_PARSE (1 << 2)
#define CABOR_ARG_SERVER (1 << 3)
#define CABOR_ARG_COMPILE (1 << 4)

static unsigned int parse_cmd_args(int argc, char** argv, int* tokenize_arg, int* parse_arg, int* compile_arg)
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

		if (!strcmp(arg, "--server") || !strcmp(arg, "-s"))
		{
			bit_flags |= CABOR_ARG_SERVER;
		}

		if (!strcmp(arg, "--compile") || !strcmp(arg, "-c"))
		{
			bit_flags |= CABOR_ARG_COMPILE;
			*compile_arg = i + 1;
		}
	}

	return bit_flags;
}

static void run_tokenizer(const char* filename)
{
	cabor_file* file = cabor_load_file(filename);
	cabor_vector* tokens = cabor_tokenize(file);

	size_t buffer_size = tokens->size * sizeof(cabor_token);
	cabor_allocation buffer = CABOR_MALLOC(buffer_size);
	cabor_stringify_tokens(buffer.mem, buffer_size, tokens);
	CABOR_LOG_F("%s", buffer);

	cabor_destroy_file(file);
	cabor_destroy_vector(tokens);

	CABOR_FREE(&buffer);
}

static void run_parser(const char* filename)
{
	// TODO
}

static void run_server()
{
	cabor_server_context ctx;
	cabor_start_compile_server(&ctx);
}

int main(int argc, char **argv) 
{
#if _DEBUG && WIN32
int CRTDBFLAGS = _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF;
_CrtSetDbgFlag(CRTDBFLAGS);
#endif

	CABOR_CREATE_ALLOCATOR();
	CABOR_INITIALIZE_TEST_FRAMEWORK();
	CABOR_CREATE_LOGGER();

	int tokenize_arg;
	int parse_arg;
	int compile_arg;

	unsigned int flags = parse_cmd_args(argc, argv, &tokenize_arg, &parse_arg, &compile_arg);
	unsigned int test_results = 0;

	if (flags & CABOR_ARG_ENABLE_TESTING)
	{
		register_all_tests();
		double start = cabor_get_time();
		test_results = CABOR_RUN_ALL_TESTS();
		double end = cabor_get_time();
		double diff_ms = (end - start) * 1000;
		CABOR_LOG_TEST_F("Finished testing in %.3f ms", diff_ms);
	}

	if (flags & CABOR_ARG_TOKENIZE)
	{
		run_tokenizer(argv[tokenize_arg]);
	}

	if (flags & CABOR_ARG_PARSE)
	{
		run_parser(argv[tokenize_arg]);
	}

	if (flags & CABOR_ARG_COMPILE)
	{
		const char* filename = argv[compile_arg];
		cabor_file* code = cabor_load_file(filename);
		cabor_x64_assembly* asmbl = cabor_compile(code->file_memory.mem, filename);
		cabor_destroy_x64_assembly(asmbl);
	}

	if (flags & CABOR_ARG_SERVER)
	{
		run_server();
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