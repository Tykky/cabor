#include "tokenizer.h"

#include "../logging/logging.h"
#include "../debug/cabor_debug.h"

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static const char* cabor_keywords[] = { "if", "then", "else", "while", "return", "for", "do", "var"};
static const size_t cabor_keyword_count = sizeof(cabor_keywords) / sizeof(cabor_keywords[0]);

static bool check_for_ignored_characters(const char c)
{
    return (c == '\n' || c == ' ' || c == '\r');
}

static void copy_to_out_token(size_t cursor, size_t size, size_t token_cursor, char* temp_token, cabor_token* out_token, cabor_token_type type)
{
    // The token cursor that gets passed here is past the last character in the token so technically it is the size
    if (token_cursor < CABOR_TOKENIZER_MAX_TOKEN_LENGTH)
    {
        // Copy results to out_token and add null terminator
        CABOR_ASSERT(token_cursor < CABOR_TOKENIZER_MAX_TOKEN_LENGTH, "Max token length exceeded!");
        memcpy(out_token->data, temp_token, token_cursor);
        out_token->type = type;
        out_token->data[token_cursor] = '\0';
    }
    else
    {
        CABOR_LOG_ERR_F("Token size was %d when max size is %d", token_cursor + 1, CABOR_TOKENIZER_MAX_TOKEN_LENGTH);
        CABOR_RUNTIME_ERROR("token is larger than CABOR_TOKENIZER_MAX_TOKEN_LENGTH!");
    }
}

static size_t match_comment(const char* buffer, size_t cursor, size_t size)
{
    char c = buffer[cursor];

    // No space left so there cant be comment
    if (size - cursor < 2)
        return cursor;

    // Line comment, find new line and return cursor to the next character after that
    if (c == '/' && buffer[cursor + 1] == '/')
    {
        for (size_t i = cursor + 2; i < size; i++)
        {
            // next character
            char nc = buffer[i];
            if (nc == '\n')
            {
                return i + 1;
            }
        }
        CABOR_LOG_WARN("No new line found after line comment");
        return cursor;
    }

    // Block comment, find ending token and return character after that
    if (c == '/' && buffer[cursor + 1] == '*')
    {
        for (size_t i = cursor + 2; i < size; i++)
        {
            // next character
            char nc = buffer[i];
            
            // Not enough characters left raise error
            if (size - i < 2)
            {
                CABOR_LOG_WARN("Not enough characters to close block comment!");
                return cursor;
            }
        
            // next next character
            char nnc = buffer[i + 1];
            if (nc == '*' && nnc == '/')
            {
                return i + 2;
            }
        }
        CABOR_LOG_WARN("No closing token found for block comment!");
    }

    return cursor;
}

static size_t match_punctuation(const char* buffer, size_t cursor, size_t size, cabor_token* out_token)
{
    char c = buffer[cursor];

    if (c == '(' || c == ')' || c == '{' || c == '}' || c == ',' || c == ';' || c == ':')
    {
        out_token->data[0] = c;
        out_token->type = CABOR_PUNCTUATION;
        return cursor + 1;
    }

    return cursor;
}

static size_t match_operator(const char* buffer, size_t cursor, size_t size, cabor_token* out_token)
{
    char token[CABOR_TOKENIZER_MAX_TOKEN_LENGTH] = {0};
    size_t token_cursor = 0;

    size_t i = cursor;

    bool or_matched = false;
    bool and_mateched = false;
    bool not_matched = false;

    while (cursor < size)
    {
        char c = buffer[cursor];
        bool is_char_valid = false;

        // Double character operator matched, e.g !=, <=, >=
        bool dc_matched = false;

        char prev_c = '\0';

        // Check for double character operators
        if (prev_c == '=' && c == '=')
        {
            dc_matched = true;
        }
        else if (prev_c == '!' && c == '=')
        {
            dc_matched = true;
        }
        else if (prev_c == '<' && c == '=')
        {
            dc_matched = true;
        }

        // Not the nicest way to handle this but since character 'o' and 'a' can be
        // part of identifiers or operators we need to be extra careful when matching these.

        if (c == 'o' && cursor + 1 < size)
        {
            char second = buffer[cursor + 1];
            if (second == 'r')
            {
                token[token_cursor++] = c;
                token[token_cursor++] = second;
                or_matched = true;
                is_char_valid = true;
                ++cursor;
            }
        }

        if (c == 'a' && cursor + 2 < size)
        {
            char second = buffer[cursor + 1];
            char third = buffer[cursor + 2];
            if (second == 'n' && third == 'd')
            {
                token[token_cursor++] = c;
                token[token_cursor++] = second;
                token[token_cursor++] = third;
                and_mateched = true;
                is_char_valid = true;
                cursor += 2;
            }
        }

        if (c == 'n' && cursor + 2 < size)
        {
            char second = buffer[cursor + 1];
            char third = buffer[cursor + 2];
            if (second == 'o' && third == 't')
            {
                token[token_cursor++] = c;
                token[token_cursor++] = second;
                token[token_cursor++] = third;
                not_matched = true;
                is_char_valid = true;
                cursor += 2;
            }
        }

        if (dc_matched)
        {
            token[token_cursor++] = c;
            is_char_valid = true;
            cursor++;
            break;
        }

        // Match single character operators
        if (prev_c == '\0' && (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>' || c == '!' || c == '%'))
        {
            token[token_cursor++] = c;
            is_char_valid = true;
            prev_c = c;
        }

        if (!is_char_valid)
            break;

        cursor++;
    }

    // if these were or/and we need to make sure we got exactly 2 or 3 characters
    // if not these are identifiers and not operators

    if (or_matched && token_cursor != 2)
    {
        return cursor;
    }

    if (and_mateched && token_cursor != 3)
    {
        return cursor;
    }

    if (not_matched && token_cursor != 3)
    {
        return cursor;
    }

    if (token_cursor == 0)
        return cursor;

    copy_to_out_token(cursor, size, token_cursor, token, out_token, CABOR_OPERATOR);

    return cursor;

}

static size_t match_integer_literal(const char* buffer, size_t cursor, size_t size, cabor_token* out_token)
{
    char token[CABOR_TOKENIZER_MAX_TOKEN_LENGTH] = {0};
    size_t token_cursor = 0;

    bool is_integer = false;

    while (cursor < size)
    {
        char c = buffer[cursor];

        bool is_char_valid = false;

        // Match integers 0-9
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

    // If this is just singular - sign then this is not integer literal but operator instead
    if (token_cursor == 1 && token[0] == '-')
    {
        return --cursor;
    }

    copy_to_out_token(cursor, size, token_cursor, token, out_token, CABOR_INTEGER_LITERAL);

    return cursor;
}

// Figure out if the identifier is keyword or just regular identifier. Keywords
// are special non user defined identifiers such as if, else etc...
static cabor_token_type get_token_type_identifier_or_keyword(const char* word)
{
    for (size_t i = 0; i < cabor_keyword_count; i++)
    {
        char* kword = cabor_keywords[i];
        if (strcmp(kword, word) == 0)
        {
            return CABOR_KEYWORD;
        }
    }
    return CABOR_IDENTIFIER;
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

    cabor_token_type token_type = get_token_type_identifier_or_keyword(token);

    copy_to_out_token(cursor, size, token_cursor, token, out_token, token_type);

    return i;
}

// Append only valid tokens
static void append_token(cabor_vector* vec, cabor_token* token)
{
    cabor_vector_push_token(vec, token);
}

size_t cabor_get_token_size()
{
    return sizeof(cabor_token);
}

bool is_match(cabor_token* token)
{
    return token->type != CABOR_TOKEN_UNKNOWN;
}

bool is_match_comment(size_t old_cursor, size_t new_cursor)
{
    return new_cursor > old_cursor;
}

cabor_vector* cabor_tokenize(cabor_file* file)
{
    cabor_vector* vector = cabor_create_vector(CABOR_TOKENIZER_VECTOR_DEFAULT_CAPACITY, CABOR_TOKEN, true);

    char* buffer = file->file_memory.mem;
    size_t size = file->size;

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

        // Skips cursor after comment if comment is found
        size_t new_cursor = match_comment(buffer, cursor, size);

        if (is_match_comment(cursor, new_cursor))
        {
            cursor = new_cursor;
            continue;
        }

        cursor = match_operator(buffer, cursor, size, &token);
        if (is_match(&token))
        {
            append_token(vector, &token);
            continue;
        }

        cursor = match_identifier(buffer, cursor, size, &token);
        if (is_match(&token))
        {
            append_token(vector, &token);
            continue;
        }

        cursor = match_integer_literal(buffer, cursor, size, &token);
        if (is_match(&token))
        {
            append_token(vector, &token);
            continue;
        }

        cursor = match_punctuation(buffer, cursor, size, &token);
        if (is_match(&token))
        {
            append_token(vector, &token);
            continue;
        }

        // if we end up here there were no matches
       CABOR_LOG_WARN_F("Tokenizer encountered character that did not match to anything, the character: %c", c);
        cursor++; // skip over the unmatched character
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
