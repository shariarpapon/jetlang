#pragma once

typedef enum jet_log_level
{
    JET_LOG_LEVEL_FATAL = 0,
    JET_LOG_LEVEL_ERROR = 1,
    JET_LOG_LEVEL_WARNING = 2,
    JET_LOG_LEVEL_INFO = 3,
    JET_LOG_LEVEL_DEBUG = 4,
};

void jet_log_output(jet_log_level level, const char* fmt, ...);

#define JET_LOG_ENABLE_WARNING 1
#define JET_LOG_ENABLE_INFO 1
#define JET_LOG_ENABLE_DEBUG 1

#define JET_LOG





