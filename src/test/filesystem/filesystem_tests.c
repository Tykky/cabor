#include "filesystem_tests.h"

#ifdef CABOR_ENABLE_TESTING

#include "../../filesystem/filesystem.h"
#include "../../logging/logging.h"
#include "../test_framework.h"


int cabor_test_load_file()
{
    const char* filename = "cabor_test_programs/test.cc";
    const char* expected = "hello XD";

    cabor_file file = cabor_load_file(filename);

    int res = 0;
    CABOR_CHECK_EQUALS(file.file_memory.size, strlen(expected), res);

    for (size_t i = 0; i < file.file_memory.size; i++)
    {
        char a = cabor_read_byte_from_file(&file, i);
        char b = expected[i];
        CABOR_CHECK_EQUALS(a, b, res);
    }

    cabor_destroy_file(&file);

    return res;
}

#endif // CABOR_ENABLE_TESTING
