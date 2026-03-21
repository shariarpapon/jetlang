#include <jet_err_handler.h>
#include <jet_logger.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

void jet_err_handler_push(
        const char* filename, 
        const jet_span* span, 
        const char* msg)
{
    JET_ASSERT(filename != NULL);
    JET_ASSERT(span != NULL);
    if(msg)
        JET_LOG_ERROR(JET_ERR_HEADER_FMT " %s", JET_ERR_HEADER_ARGS(filename, span), msg);
    else
        JET_LOG_ERROR(JET_ERR_HEADER_FMT, JET_ERR_HEADER_ARGS(filename, span));
}

void jet_err_handler_pushf(
        const char* filename, 
        const jet_span* span, 
        const char* fmt, ...)
{
    JET_ASSERT(fmt != NULL);
    JET_ASSERT(filename != NULL);
    JET_ASSERT(span != NULL);

    va_list args;
    va_start(args, fmt);
    char buf[JET_LOG_MSG_BUF_SIZE];
    int written = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    JET_ASSERTM(written >= 0, "vsnprintf failed");

    if((size_t)written >= sizeof(buf))
    { 
        size_t end = sizeof(buf) - 4;
        buf[end] = '\0';
        JET_LOG_ERROR(JET_ERR_HEADER_FMT " %s...", JET_ERR_HEADER_ARGS(filename, span), buf);
    }
    else 
        JET_LOG_ERROR(JET_ERR_HEADER_FMT " %s", JET_ERR_HEADER_ARGS(filename, span), buf);
}
