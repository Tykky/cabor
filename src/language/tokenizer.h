#pragma once

#include "../core/vector.h"
#include "../filesystem/filesystem.h"

#define CABOR_TOKENIZER_VECTOR_DEFAULT_CAPACITY 1024
#define CABOR_TOKENIZER_MAX_TOKEN_LENGTH 64

typedef enum
{
    CABOR_IDENTIFIER,
    CABOR_INTEGER_LITERAL,
    CABOR_OPERATOR,
    CABOR_PUNCTUATION,
    CABOR_KEYWORD,
    CABOR_UNIT,
    CABOR_TOKEN_UNKNOWN,
} cabor_token_type;

typedef struct cabor_token_t
{
    cabor_token_type type;
    char data[CABOR_TOKENIZER_MAX_TOKEN_LENGTH];
} cabor_token;

size_t cabor_get_token_size();

cabor_vector* cabor_tokenize(cabor_file* file);

void cabor_stringify_tokens(char* buffer, size_t size, cabor_vector* tokens);
