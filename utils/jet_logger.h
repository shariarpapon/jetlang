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
void jet_assert(bool cond, const char* expr, const char* msg, const char* file, int line);

#define JET_LOG_ENABLE_WARNING  1
#define JET_LOG_ENABLE_INFO     1
#define JET_LOG_ENABLE_DEBUG    1
#define JET_ASSERTION_ENABLED

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


#ifdef JET_ASSERTION_ENABLED

    #if _MSC_VER
        #include <intrin.h>
        #define JET_DEBUG_BREAK() __debugBreak()
    #else
        #define JET_DEBUG_BREAK() __builtin_trap()
    #endif

    #define JET_ASSERT(expr) \
        jet_assert(expr, #expr, NULL, __FILE__, __LINE__)

    #define JET_ASSERTM(expr, msg) \
        jet_assert(expr, #expr, msg, __FILE__, __LINE__)
#else
    #define JET_ASSERT(expr)
    #define JET_ASSERTM(expr, msg)
#endif

#ifdef JET_LOGGER_IMPL

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#define MSG_BUF_SIZE (1024 * 32) 
#define OUT_BUF_SIZE (MSG_BUF_SIZE + 64)

static void jet_assert_fail(const char* expr, const char* msg, const char* file, int line);

void jet_log_output(jet_log_level level, const char* fmt, ...)
{
    if(level < JET_LOG_LEVEL_FATAL) 
        level = JET_LOG_LEVEL_FATAL;
    else if(level > JET_LOG_LEVEL_DEBUG) 
        level = JET_LOG_LEVEL_DEBUG; 

    const char* level_str[] = { "FATAL", "ERROR", "WARNING", "INFO", "DEBUG" };
    va_list args;
    va_start(args, fmt);
    
    char msg_buf[MSG_BUF_SIZE];
    memset(msg_buf, 0, sizeof(msg_buf));
    vsnprintf(msg_buf, sizeof(msg_buf), fmt, args); 
    va_end(args);

    char out_buf[OUT_BUF_SIZE];
    memset(out_buf, 0, sizeof(out_buf));
    snprintf(out_buf, sizeof(out_buf), "[%s] %s\n", level_str[level], msg_buf);
    printf("%s", out_buf);
}

void jet_assert(bool cond, const char* expr, const char* msg, const char* file, int line)
{
    if(cond) 
        return;
    jet_assert_fail(expr, msg, file, line);
}

static void jet_assert_fail(const char* expr, const char* msg, const char* file, int line)
{
    if(msg) jet_log_output(JET_LOG_LEVEL_FATAL, 
               "ASSERTION FAILED (%s)\n\tfile: %s, line: %d \n\tmsg: %s\n", 
                expr, file, line, msg);
    else jet_log_output(JET_LOG_LEVEL_FATAL, 
               "ASSERTION FAILED (%s)\n\tfile: %s, line: %d\n", 
                expr, file, line);
    JET_DEBUG_BREAK();
}

#endif
