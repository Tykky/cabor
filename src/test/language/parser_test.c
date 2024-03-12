#include "parser_test.h"

#include "../../core/vector.h"
#include "../../language/tokenizer.h"

#include <string.h>

#ifdef CABOR_ENABLE_TESTING

// Normally we produce tokens from a file but for testing we are creating
// them manually here
static cabor_token create_token(const char* data, cabor_token_type type)
{
    cabor_token t;
    t.type = type;
    size_t size = strlen(data);
    strcpy_s(t.data, 100, data);
    return t;
}

int cabor_test_parse_tokens()
{
    cabor_vector tokens = create_cabor_vector(100, CABOR_TOKEN, true);

    cabor_token tmp[5] = {0};
    size_t i = 0;

    tmp[i++] = create_token("a", CABOR_IDENTIFIER);
    tmp[i++] = create_token("+", CABOR_OPERATOR);
    tmp[i++] = create_token("b", CABOR_IDENTIFIER);
    tmp[i++] = create_token("*", CABOR_OPERATOR);
    tmp[i++] = create_token("c", CABOR_IDENTIFIER);

    for (size_t i = 0; i < 5; i++)
    {
        cabor_vector_push_token(&tokens, &tmp[i]);
    }

    char token_string[100] = {0};

    cabor_stringify_tokens(token_string, 100, &tokens);

    const char* expected_tokens = "['a', '+', 'b', '*', 'c']";

    int test = strcmp(expected_tokens, token_string);

    if (test)
    {
        CABOR_LOG_ERR_F("EXPECTED: %s", expected_tokens);
        CABOR_LOG_ERR_F("ACTUAL  : %s", token_string);
    }

    int res = 0;
    CABOR_CHECK_EQUALS(test, 0, res);

    destroy_cabor_vector(&tokens);

    return res;
}

#endif // CABOR_ENABLE_TESTING
