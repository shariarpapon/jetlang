#include <jet_err_handler.h>
#include <jet_logger.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

static bool handler_started = false;
static const char* cur_filename = NULL;
static size_t err_count = 0;

#define JET_ERR_LOG(span, msg, ...) \
        jet_log_outputf_flc(JET_LOG_LEVEL_ERROR, cur_filename, span->line, span->col, msg, ##__VA_ARGS__)

void jet_err_handler_start(const char* filename)
{
    JET_ASSERTM(filename != NULL, "cannot start err handler, must provide a filename.");
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

bool jet_err_handler_is_started() { return handler_started; }
const char* jet_err_handler_get_filename() { return cur_filename; }
size_t jet_err_handler_get_count() { return err_count; }

void jet_err_handler_pushf(const jet_span* span, const char* fmt, ...) 
{
    if(!handler_started)
    {
        JET_LOG_ERROR("must start error handler before pushing errors.");
        return;
    }

    if(!cur_filename || !fmt || !span)
    {
        JET_LOG_ERROR("cannot push error, one or more invalid args.");
        return;
    }

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
        JET_ERR_LOG(span, "%s...", buf);
    }
    else JET_ERR_LOG(span, "%s", buf);
    err_count++;
}

void jet_err_handler_push(const jet_span* span, const char* msg)
{
    if(!handler_started)
    {
        JET_LOG_ERROR("must start error handler before pushing errors.");
        return;
    }
    if(!span)
    {
        JET_LOG_ERROR("cannot push error, null arg<span>");
        return;
    }
    jet_err_handler_pushf(span, "%s", msg);
}

