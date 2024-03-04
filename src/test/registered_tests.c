#include "registered_tests.h"

#ifdef CABOR_ENABLE_TESTING

#include "core/vector_test.h"
#include "filesystem/filesystem_tests.h"

void register_all_tests()
{
    CABOR_REGISTER_TEST("test_vector_push", cabor_test_vector_push);
    CABOR_REGISTER_TEST("test_vector_get", cabor_test_vector_get);
    CABOR_REGISTER_TEST("test_vector_peek", cabor_test_vector_peek);
    CABOR_REGISTER_TEST("test_vector_resize", cabor_test_vector_resize);
    CABOR_REGISTER_TEST("test_vector_zero_initialized", cabor_test_zero_vector_initialized);
    CABOR_REGISTER_TEST("test_cabor_load_file", cabor_test_load_file);
}

#endif
