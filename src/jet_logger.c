#include <jet_logger.h>

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
