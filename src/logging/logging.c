#include "logging.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifdef CABOR_ENABLE_LOGGING

#define CABOR_ANSI_COLOR_RED     "\x1b[31m"
#define CABOR_ANSI_COLOR_GREEN   "\x1b[32m"
#define CABOR_ANSI_COLOR_YELLOW  "\x1b[33m"
#define CABOR_ANSI_COLOR_BLUE    "\x1b[34m"
#define CABOR_ANSI_COLOR_MAGENTA "\x1b[35m"
#define CABOR_ANSI_COLOR_CYAN    "\x1b[36m"
#define CABOR_ANSI_COLOR_RESET   "\x1b[0m"

static cabor_logging_context g_logging_ctx;

void create_cabor_logger(cabor_logging_context* logging_context)
{
    logging_context->log_buffer = cabor_create_vector(CABOR_LOGGER_INITIAL_BUFFER_SIZE, CABOR_CHAR, false);
    logging_context->log_buffer_lock = cabor_create_mutex();
}

void destroy_cabor_logger(cabor_logging_context* logging_context)
{
    cabor_destroy_vector(logging_context->log_buffer);
    cabor_destroy_mutex(logging_context->log_buffer_lock);
}

void push_log(const char* message, cabor_log_type type)
{
    //cabor_lock(g_logging_ctx.log_buffer_lock);
    //CABOR_SCOPED_LOCK(g_logging_ctx.log_buffer_lock)
    //{
        size_t i = 0;
        char newline = '\n';

        switch (type)
        {
        case CABOR_TRACE:
        {
            const char* trace_prefix = "[TRACE] ";
            const char* trace_prefix_colored = CABOR_ANSI_COLOR_RESET "[TRACE] ";
            cabor_vector_push_str(g_logging_ctx.log_buffer, trace_prefix, false);
            fputs(trace_prefix_colored, stdout);
            break;
        }
        case CABOR_WARNING:
        {
            const char* warning_prefix = "[WARNING] ";
            const char* warning_prefix_colored = CABOR_ANSI_COLOR_YELLOW "[WARNING] ";
            cabor_vector_push_str(g_logging_ctx.log_buffer, warning_prefix, false);
            fputs(warning_prefix_colored, stdout);
            break;
        }
        case CABOR_ERROR:
        {
            const char* error_prefix = "[ERROR] ";
            const char* error_prefix_colored = CABOR_ANSI_COLOR_RED "[ERROR] ";
            cabor_vector_push_str(g_logging_ctx.log_buffer, error_prefix, false);
            fputs(error_prefix_colored, stdout);
            break;
        }
        case CABOR_TEST:
        {
            const char* test_prefix = "[TEST] ";
            const char* test_prefix_colored = CABOR_ANSI_COLOR_MAGENTA "[TEST] ";
            cabor_vector_push_str(g_logging_ctx.log_buffer, test_prefix, false);
            fputs(test_prefix_colored, stdout);
            break;
        }
        case CABOR_BLANK:
        {
            break;
        }
        }

        cabor_vector_push_str(g_logging_ctx.log_buffer, message, true);
        cabor_vector_push_char(g_logging_ctx.log_buffer, newline);
        puts(message);
        fputs(CABOR_ANSI_COLOR_RESET, stdout);
    //}
    //cabor_unlock(g_logging_ctx.log_buffer_lock);
}

void push_log_f(const char* message, cabor_log_type type, ...)
{
    //cabor_lock(g_logging_ctx.log_buffer_lock);
    //CABOR_SCOPED_LOCK(g_logging_ctx.log_buffer_lock)
    //{
        char buffer[CABOR_LOGGER_FORMAT_STR_BUFFER_SIZE] = { 0 };
        size_t msg_len = strlen(message);

        va_list args;
        va_start(args, type);
        int result = vsnprintf(buffer, CABOR_LOGGER_FORMAT_STR_BUFFER_SIZE, message, args);
        va_end(args);

        if (result <= 0)
        {
            fputs("Failed to push format string to the temp buffer! buffer is too small", stderr);
            exit(1);
        }

        push_log(buffer, type);
    //}
    //cabor_unlock(g_logging_ctx.log_buffer_lock);
}

cabor_logging_context* get_cabor_global_logging_context()
{
    return &g_logging_ctx;
}

void dump_cabor_log_to_disk(cabor_logging_context* ctx, const char* filename)
{
    //cabor_lock(g_logging_ctx.log_buffer_lock);
    //CABOR_SCOPED_LOCK(g_logging_ctx.log_buffer_lock)
    //{
        FILE* fp = fopen(filename, "ab");
        fwrite(ctx->log_buffer->vector_mem.mem, sizeof(char), ctx->log_buffer->size, fp);
        fclose(fp);
    //}
    //cabor_unlock(g_logging_ctx.log_buffer_lock);
}

#endif
