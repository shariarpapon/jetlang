#pragma once
#include <stdbool.h>

typedef enum jet_log_level
{
    JET_LOG_LEVEL_FATAL = 0,
    JET_LOG_LEVEL_ERROR = 1,
    JET_LOG_LEVEL_WARNING = 2,
    JET_LOG_LEVEL_INFO = 3,
    JET_LOG_LEVEL_DEBUG = 4,
} jet_log_level;

void jet_log_output(jet_log_level level, const char* filename, 
                    int line, const char* fmt, ...);

void jet_assert(bool cond, const char* expr, const char* msg, 
                const char* file, int line);

#define JET_LOG_MSG_BUF_SIZE (1024 * 32) 

#define JET_LOG_ENABLE_WARNING 1 
#define JET_LOG_ENABLE_INFO    1 
#define JET_LOG_ENABLE_DEBUG   1 
#define JET_ASSERTION_ENABLED


#define JET_LOG_FATAL(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define JET_LOG_ERROR(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#if JET_LOG_ENABLE_WARNING == 1
#define JET_LOG_WRN(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_WARNING, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define JET_LOG_WRN(fmt, ...) ((void)0)
#endif

#if JET_LOG_ENABLE_INFO == 1
#define JET_LOG_INFO(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define JET_LOG_INFO(fmt, ...) ((void)0)
#endif

#if JET_LOG_ENABLE_DEBUG == 1
#define JET_LOG_DEBUG(fmt, ...) \
    jet_log_output(JET_LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else 
#define JET_LOG_DEBUG(fmt, ...) ((void)0)
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
    #define JET_ASSERT(expr) ((void)0)
    #define JET_ASSERTM(expr, msg) ((void)0)
#endif

#ifdef JET_LOGGER_IMPL

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

static void jet_assert_fail(const char* expr, const char* msg, const char* file, int line);

void jet_log_output(jet_log_level level, const char* filename, int line, const char* fmt, ...)
{
    if(level < JET_LOG_LEVEL_FATAL) 
        level = JET_LOG_LEVEL_FATAL;
    else if(level > JET_LOG_LEVEL_DEBUG) 
        level = JET_LOG_LEVEL_DEBUG; 

    const char* level_str[] = { "FATAL", "ERROR", "WARNING", "INFO", "DEBUG" };
    va_list args;
    va_start(args, fmt);
    
    char msg_buf[JET_LOG_MSG_BUF_SIZE];
    memset(msg_buf, 0, sizeof(msg_buf));
    vsnprintf(msg_buf, sizeof(msg_buf), fmt, args); 
    va_end(args);

    char out_buf[JET_LOG_MSG_BUF_SIZE + 64];
    memset(out_buf, 0, sizeof(out_buf));
    snprintf(out_buf, sizeof(out_buf), "[%s] [%s:%d] %s\n", level_str[level], filename, line, msg_buf);
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
    if(msg) 
        jet_log_output(JET_LOG_LEVEL_FATAL, file, line, 
                "ASSERTION FAILED(%s) : %s", expr, msg);
    else
        jet_log_output(JET_LOG_LEVEL_FATAL, file, line, 
                "ASSERTION FAILED(%s)", expr);
    fflush(stdout);
    JET_DEBUG_BREAK();
}

#endif
