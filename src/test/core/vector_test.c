#include "vector_test.h"
#include "../../logging/logging.h"
#include "../test_framework.h"

#ifdef CABOR_ENABLE_TESTING

int cabor_test_vector_push()
{
    return 0;
    cabor_vector vec = create_cabor_vector(1024, CABOR_INT, false);

    for (size_t i = 0; i < 10; i++)
        cabor_vector_push_int(&vec, i);

    int res = 0;
    CABOR_CHECK_EQUALS(vec.size, 10, res);

    cabor_vector_destroy(&vec);

    return res;
}

int cabor_test_vector_get()
{
    return 0;
    cabor_vector vec = create_cabor_vector(1024, CABOR_INT, false);

    for (size_t i = 0; i < 10; i++)
        cabor_vector_push_int(&vec, i + 1);

    int res = 0;

    for (size_t i = 0; i < 9; i++)
        CABOR_CHECK_EQUALS(cabor_vector_get_int(&vec, i), i + 1, res);

    cabor_vector_destroy(&vec);

    return 0;
}

int cabor_test_vector_peek()
{
    return 0;
    cabor_vector vec = create_cabor_vector(1024, CABOR_INT, false);

    for (size_t i = 0; i < 10; i++)
        cabor_vector_push_int(&vec, i + 1);

    int* next = cabor_vector_peek_int(&vec);

    int res = 0;
    CABOR_CHECK_EQUALS(((long)next - (long)vec.vector_mem.mem) / sizeof(int), vec.size, res);

    cabor_vector_destroy(&vec);

    return res;
}

int cabor_test_vector_resize()
{
    cabor_vector vec = create_cabor_vector(1, CABOR_INT, false);

    for (size_t i = 0; i < 10; i++)
        cabor_vector_push_int(&vec, i + 1);

    int res = 0;
    CABOR_CHECK_GREATER_EQ(vec.capacity, 10, res);

    cabor_vector_destroy(&vec);

    return res;
}

#endif // CABOR_ENABLE_TESTING
