#pragma once

typedef enum jet_log_level
{
    JET_LOG_LEVEL_FATAL = 0,
    JET_LOG_LEVEL_ERROR = 1,
    JET_LOG_LEVEL_WARNING = 2,
    JET_LOG_LEVEL_INFO = 3,
    JET_LOG_LEVEL_DEBUG = 4,
} jet_log_level;

void jet_log_output(jet_log_level level, const char* fmt, ...);

#define JET_LOG_ENABLE_WARNING  1
#define JET_LOG_ENABLE_INFO     1
#define JET_LOG_ENABLE_DEBUG    1

#define JET_FATAL(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)


#define JET_ERROR(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)


#if JET_LOG_ENABLE_WARNING == 1
#define JET_WARNING(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_WARNING, fmt, ##__VA_ARGS__)
#else
#define JET_WARNING(fmt, ...)
#endif

#if JET_LOG_ENABLE_INFO == 1
#define JET_INFO(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#define JET_INFO(fmt, ...)
#endif

#if JET_LOG_ENABLE_DEBUG == 1
#define JET_DEBUG(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else 
#define JET_DEBUG(fmt, ...)
#endif


#ifdef JET_LOGGER_IMPL

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define MSG_BUF_SIZE (1024 * 32) 

void jet_log_output(jet_log_level level, const char* fmt, ...)
{
    const char* level_str[] = { "FATAL", "ERROR", "WARNING", "INFO", "DEBUG" };
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    
    char msg_buf[MSG_BUF_SIZE];
    memset(msg_buf, 0, sizeof(msg_buf));
    vsnprintf(msg_buf, sizeof(msg_buf) + 32, fmt, args); 
    __builtin_va_end(args);

    char out_buf[MSG_BUF_SIZE];
    memset(out_buf, 0, sizeof(out_buf));
    snprintf(out_buf, sizeof(out_buf) + 32, "[%s] %s\n", level_str[level], msg_buf);

    printf("%s", out_buf);
}

#endif
