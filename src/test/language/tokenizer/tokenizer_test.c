#include "tokenizer_test.h"

#include "../../../filesystem/filesystem.h"
#include "../../../logging/logging.h"
#include "../../../debug/cabor_debug.h"

#include <stdio.h>
#include <string.h>

#ifdef CABOR_ENABLE_TESTING

#define CABOR_TOKEN_STRINGIFY_STR_SIZE 1024

int cabor_test_tokenize_hello_world()
{
    cabor_file file = cabor_load_file("cabor_test_programs/hello_world.cc");

    const char* expected = "['if', '(', 'a', '<=', 'bee', ')', '{', 'print', '(', '420', '-', '123', ')', ';', '}', 'if', '(', 'c', '>=', 'men', ')', '{', 'print', '(', '2', '*', '2', ')', ';', '}']";

    cabor_vector tokens = cabor_tokenize(&file);
    char token_string[CABOR_TOKEN_STRINGIFY_STR_SIZE] = { 0 };

    cabor_stringify_tokens(token_string, CABOR_TOKEN_STRINGIFY_STR_SIZE, &tokens);

    int cmp_result = strcmp(token_string, expected);
    int res = 0;

    if (cmp_result)
    {
        CABOR_LOG_ERR_F("EXPECTED : %s", expected);
        CABOR_LOG_ERR_F("ACTUAL   : %s", token_string);
    }

    CABOR_CHECK_EQUALS(cmp_result, 0, res);

    destroy_cabor_vector(&tokens);
    cabor_destroy_file(&file);

    return res;
}


#endif // CABOR_ENABLE_TESTING
