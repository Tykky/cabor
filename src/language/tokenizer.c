#include "tokenizer.h"

#include "../logging/logging.h"
#include "../debug/cabor_debug.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static bool check_for_ignored_characters(const char c)
{
    return (c == '\n' || c == ' ' || c == '\r');
}

static void copy_to_out_token(size_t cursor, size_t size, size_t token_cursor, char* temp_token, cabor_token* out_token, cabor_token_type type)
{
    // Copy results to out_token and add null terminator
    CABOR_ASSERT(size - cursor >= token_cursor + 2, "Not enough space to fit token and null terminator!");
    memcpy(out_token->data, temp_token, token_cursor + 1);
    out_token->type = type;
    out_token->data[token_cursor + 1] = '\0';
}

static size_t match_integer_literals(const char* buffer, size_t cursor, size_t size, cabor_token* out_token)
{
    char token[CABOR_TOKENIZER_MAX_TOKEN_LENGTH] = {0};
    size_t token_cursor = 0;

    bool is_integer = false;

    size_t i = cursor;

    while (cursor < size)
    {
        char c = buffer[cursor];

        bool is_char_valid = false;

        if (c >= '0' && c <= '9')
        {
            token[token_cursor++] = c;
            is_integer = true;
            is_char_valid = true;
        }

        // We only accept one minus sign
        if (!is_integer && c == '-')
        {
            token[token_cursor++] = c;
            is_char_valid = true;
            cursor++;
            break;
        }

        if (!is_char_valid)
            break;

        cursor++;
    }

    if (token_cursor == 0)
        return cursor;

    copy_to_out_token(cursor, size, token_cursor, token, out_token, CABOR_INTEGER_LITERAL);

    return cursor;
}

// Return the new cursor position if it was a match otherwise return the original cursor.
static size_t match_identifier(const char* buffer, size_t cursor, size_t size, cabor_token* out_token)
{
    char token[CABOR_TOKENIZER_MAX_TOKEN_LENGTH] = {0};
    size_t token_cursor = 0;

    bool first_char_is_letter = false;

    size_t i = cursor;

    while(i < size)
    {
        char c = buffer[i];
        bool char_is_valid = false;

        // Match A-Z, a-z and _
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
        {
            if (i == cursor)
                first_char_is_letter = true;

            token[token_cursor++] = c;
            char_is_valid = true;
        }
        
        // Match 0-9
        if (first_char_is_letter && (c >= '0' && c <= '9'))
        {
            token[token_cursor++] = c;
            char_is_valid = true;
        }

        if (!char_is_valid)
            break;

        i++;
    }

    if (token_cursor == 0)
        return cursor;

    copy_to_out_token(cursor, size, token_cursor, token, out_token, CABOR_IDENTIFIER);

    return i;
}

// Append only valid tokens
static void append_token(cabor_vector* vec, cabor_token* token)
{
    cabor_vector_push_token(vec, token);
}

size_t get_cabor_token_size()
{
    return sizeof(cabor_token);
}

bool is_match(cabor_token* token)
{
    return token->type != CABOR_TOKEN_UNKNOWN;
}

cabor_vector cabor_tokenize(cabor_file* file)
{
    cabor_vector vector = create_cabor_vector(CABOR_TOKENIZER_VECTOR_DEFAULT_CAPACITY, CABOR_TOKEN, true);

    char* buffer = file->file_memory.mem;
    size_t size = file->file_memory.size;
    size_t cursor = 0;

    while (cursor < size)
    {
        char c = buffer[cursor];

        cabor_token token =
        {
            .data = {0},
            .type = CABOR_TOKEN_UNKNOWN
        };

        // For now we just append the first token that matches any of the checks to the vector

        if (check_for_ignored_characters(c))
        {
            cursor++;
            continue;
        }

        cursor = match_identifier(buffer, cursor, size, &token);
        if (is_match(&token))
        {
            append_token(&vector, &token);
            continue;
        }

        cursor = match_integer_literals(buffer, cursor, size, &token);
        if (is_match(&token))
        {
            append_token(&vector, &token);
            continue;
        }

        // if we end up here there were no matches
        CABOR_LOG_WARN_F("Tokenizer encountered character that did not match to anything, the character: %c", c);
        cursor++;
    }

    return vector;
}

void cabor_stringify_tokens(char* buffer, size_t size, cabor_vector* tokens)
{
    size_t cursor = 0;

    buffer[cursor++] = '[';
    for (size_t i = 0; i < tokens->size; i++)
    {
        cabor_token* t = cabor_vector_get_token(tokens, i);
        if (i == tokens->size - 1)
        {
            cursor += snprintf(buffer + cursor, size - cursor, "'%s'", t->data);
        }
        else
        {
            cursor += snprintf(buffer + cursor, size - cursor, "'%s', ", t->data);
        }
    }

    buffer[cursor++] = ']';
    buffer[cursor++] = '\0';

    CABOR_ASSERT(cursor < size, "cabor stringify tokens buffer overflow!");
}
