#include "logging.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

static cabor_logging_context g_logging_ctx;

void create_cabor_logger(cabor_logging_context* logging_context)
{
    logging_context->log_buffer = create_vector(CABOR_LOGGER_INITIAL_BUFFER_SIZE, sizeof(char), false);
}

void destroy_cabor_logger(cabor_logging_context* logging_context)
{
    destroy_vector(&logging_context->log_buffer);
}

void push_log(const char* message, cabor_log_type type)
{
    size_t i = 0;
    char newline = '\n';

    char* current_log = peek_next(&g_logging_ctx.log_buffer);

    switch (type)
    {
        case TRACE:
        {
            const char* trace_prefix = "[TRACE]: ";
            push_string_to_vector(&g_logging_ctx.log_buffer, trace_prefix, false);
            break;
        }
        case WARNING:
        {
            const char* warning_prefix = "[WARNING]: ";
            push_string_to_vector(&g_logging_ctx.log_buffer, warning_prefix, false);
            break;
        }
        case ERROR:
        {
            const char* error_prefix = "[ERROR]: ";
            push_string_to_vector(&g_logging_ctx.log_buffer, error_prefix, false);
            break;
        }
        case TEST:
        {
            const char* test_prefix = "[TEST]: ";
            push_string_to_vector(&g_logging_ctx.log_buffer, test_prefix, false);
            break;
        }
        case BLANK:
        {
            break;
        }
    }

    push_string_to_vector(&g_logging_ctx.log_buffer, message, true);
    pushback_vector(&g_logging_ctx.log_buffer, &newline);

    puts(current_log);
}

void push_log_f(const char* message, cabor_log_type type, ...)
{
    char buffer[CABOR_LOGGER_FORMAT_STR_BUFFER_SIZE] = { 0 };
    size_t msg_len = strlen(message);

    va_list args;
    va_start(args, type);
    int result = vsnprintf(buffer, 1024, message, args);
    va_end(args);

    if (result <= 0)
    {
        perror("Failed to push format string to the temp buffer! buffer is too small");
        exit(1);
    }

    push_log(buffer, type);
}

cabor_logging_context* get_cabor_global_logging_context()
{
    return &g_logging_ctx;
}
