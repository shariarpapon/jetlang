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


