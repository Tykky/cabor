#include <stdio.h>
#include "core/vector.h"
#include "core/memory.h"
#include "test/test_framework.h"
#include "logging/logging.h"

void parse_cmd_args(int argc, char** argv)
{
	if (argc == 0)
		return;

	for (int i = 0; i < argc; i++)
	{
	}
}

int main(int argc, char **argv) 
{
	CABOR_CREATE_ALLOCATOR();
	CABOR_CREATE_LOGGER();

	size_t n = (size_t)1 << 3;

	cabor_vector v = create_vector(n, sizeof(float), true);

	for (int i = 0; i < n; i++)
	{
		float k = (float)i * 0.5f;
		pushback_vector(&v, &k);
	}

	for (int i = 0; i < n; i++)
	{
		float k = *(float*)vector_get(&v, i);
		CABOR_LOG_TEST("%f", k);
	}

	CABOR_DESTROY_LOGGER();
	CABOR_DESTROY_ALLOCATOR();

	return 0;
}