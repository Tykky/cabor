#pragma once

#include "../core/vector.h"

#define CABOR_LOGGER_INITIAL_BUFFER_SIZE 1000
#define CABOR_LOGGER_FORMAT_STR_BUFFER_SIZE 1024

// Logging without formatting
#define CABOR_LOG_TRACE(msg) push_log(msg, TRACE)
#define CABOR_LOG_WARN(msg) push_log(msg, WARNING)
#define CABOR_LOG_ERR(msg) push_log(msg, ERROR)
#define CABOR_LOG_TEST(msg) push_log(msg, TEST)
#define CABOR_LOG(msg) push_log(msg, BLANK);

// Logging with formatting
#define CABOR_LOG_TRACE_F(msg, ...) push_log_f(msg, TRACE, __VA_ARGS__)
#define CABOR_LOG_WARN_F(msg, ...) push_log_f(msg, WARNING, __VA_ARGS__)
#define CABOR_LOG_ERR_F(msg, ...) push_log_f(msg, ERROR, __VA_ARGS__)
#define CABOR_LOG_TEST(msg, ...) push_log_f(msg, TEST, __VA_ARGS__)
#define CABOR_LOG_F(msg, ...) push_log_f(msg, BLANK, __VA_ARGS__)

#define CABOR_CREATE_LOGGER() create_cabor_logger(get_cabor_global_logging_context())
#define CABOR_DESTROY_LOGGER() destroy_cabor_logger(get_cabor_global_logging_context())

typedef enum 
{
    TRACE,
    WARNING,
    ERROR,
    TEST,
    BLANK
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
