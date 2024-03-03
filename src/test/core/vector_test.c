#include "vector_test.h"
#include "../../logging/logging.h"
#include "../test_framework.h"

int cabor_test_vector_push()
{
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
}

int cabor_test_vector_peek()
{
    return 0;
}

int cabor_test_vector_resize()
{
    return 0;
}

