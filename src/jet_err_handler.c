#include <jet_err_handler.h>
#include <jet_logger.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

static bool handler_started = false;
static const char* cur_filename = NULL;
static size_t err_count = 0;

void jet_err_handler_start(const char* filename)
{
    JET_ASSERT(filename != NULL);
    cur_filename = filename;
    err_count = 0;
    handler_started = true;
}

void jet_err_handler_end()
{
    cur_filename = NULL;
    err_count = 0;
    handler_started = false;
}

void jet_err_handler_reset()
{
    err_count = 0;
}

bool jet_err_handler_is_started()
{
    return handler_started;
}

const char* jet_err_handler_get_filename()
{
    return cur_filename;
}

size_t jet_err_handler_get_count()
{
    return err_count;
}

void jet_err_handler_pushf(
        const char* filename, 
        const jet_span* span, 
        const char* fmt, ...)
{
    if(!handler_started)
    {
        JET_LOG_ERROR("must start error handler before pushing errors.");
        return;
    }

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

    err_count++;
}

void jet_err_handler_push(
        const char* filename, 
        const jet_span* span, 
        const char* msg)
{
    if(!handler_started)
    {
        JET_LOG_ERROR("must start error handler before pushing errors.");
        return;
    }

    JET_ASSERT(filename != NULL);
    JET_ASSERT(span != NULL);
    jet_err_handler_pushf(filename, span, "%s", msg);
}
