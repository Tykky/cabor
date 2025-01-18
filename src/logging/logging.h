#pragma once

#include "../cabor_defines.h"

#ifdef CABOR_ENABLE_LOGGING

#include "../core/vector.h"

#define CABOR_LOGGER_INITIAL_BUFFER_SIZE 1024
#define CABOR_LOGGER_FORMAT_STR_BUFFER_SIZE 1024

// Logging without formatting
#define CABOR_LOG_TRACE(msg) push_log(msg, CABOR_TRACE)
#define CABOR_LOG_WARN(msg) push_log(msg, CABOR_WARNING)
#define CABOR_LOG_ERR(msg) push_log(msg, CABOR_ERROR)
#define CABOR_LOG_TEST(msg) push_log(msg, CABOR_TEST)
#define CABOR_LOG(msg) push_log(msg, CABOR_BLANK);

// Logging with formatting
#define CABOR_LOG_TRACE_F(msg, ...) push_log_f(msg, CABOR_TRACE, __VA_ARGS__)
#define CABOR_LOG_WARN_F(msg, ...) push_log_f(msg, CABOR_WARNING, __VA_ARGS__)
#define CABOR_LOG_ERR_F(msg, ...) push_log_f(msg, CABOR_ERROR, __VA_ARGS__)
#define CABOR_LOG_TEST_F(msg, ...) push_log_f(msg, CABOR_TEST, __VA_ARGS__)
#define CABOR_LOG_F(msg, ...) push_log_f(msg, CABOR_BLANK, __VA_ARGS__)

#define CABOR_CREATE_LOGGER() create_cabor_logger(get_cabor_global_logging_context())
#define CABOR_DESTROY_LOGGER() destroy_cabor_logger(get_cabor_global_logging_context())

#define CABOR_DUMP_LOG_TO_DISK() dump_cabor_log_to_disk(get_cabor_global_logging_context(), "CABOR_LOG.txt");

typedef enum 
{
    CABOR_TRACE,
    CABOR_WARNING,
    CABOR_ERROR,
    CABOR_TEST,
    CABOR_BLANK
} cabor_log_type;

typedef struct
{
    cabor_vector log_buffer;
} cabor_logging_context;

void create_cabor_logger(cabor_logging_context* logging_context);

void destroy_cabor_logger(cabor_logging_context* logging_context);

// pushes null terminated string to log buffer
void push_log(const char* message, cabor_log_type type);

// pushes null terminated string to log buffer with formatting
void push_log_f(const char* message, cabor_log_type type, ...);

cabor_logging_context* get_cabor_global_logging_context();

void dump_cabor_log_to_disk(cabor_logging_context* ctx, const char* filename);

#else
#define CABOR_LOG_TRACE(msg)
#define CABOR_LOG_WARN(msg)
#define CABOR_LOG_ERR(msg)
#define CABOR_LOG_TEST(msg)
#define CABOR_LOG(msg)

#define CABOR_LOG_TRACE_F(msg, ...)
#define CABOR_LOG_WARN_F(msg, ...)
#define CABOR_LOG_ERR_F(msg, ...)
#define CABOR_LOG_TEST_F(msg, ...)
#define CABOR_LOG_F(msg, ...)

#define CABOR_CREATE_LOGGER()
#define CABOR_DESTROY_LOGGER()

#define CABOR_DUMP_LOG_TO_DISK()

#endif
