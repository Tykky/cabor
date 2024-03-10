#include "tokenizer_test.h"

#include "../../../filesystem/filesystem.h"
#include "../../../logging/logging.h"
#include "../../../debug/cabor_debug.h"

#include <stdio.h>

#ifdef CABOR_ENABLE_TESTING

#define CABOR_TOKEN_STRINGIFY_STR_SIZE 1024

int cabor_test_tokenize_hello_world()
{
    cabor_file file = cabor_load_file("cabor_test_programs/hello_world.cc");

    cabor_vector tokens = cabor_tokenize(&file);
    char token_string[CABOR_TOKEN_STRINGIFY_STR_SIZE] = { 0 };

    cabor_stringify_tokens(token_string, CABOR_TOKEN_STRINGIFY_STR_SIZE, &tokens);

    CABOR_LOG_TEST_F("-- Tokenized file %s: %s", file.filename, token_string);

    destroy_cabor_vector(&tokens);
    cabor_destroy_file(&file);

    return 0;
}


#endif // CABOR_ENABLE_TESTING
