#include <jet_diag.h>
#include <jet_logger.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

static bool handler_started = false;
static const char* cur_filename = NULL;
static size_t report_count = 0;

#define JET_DIAG_SPAN_HEADER_FMT(span) ""

void jet_diag_start(const char* filename)
{
    JET_ASSERTM(filename != NULL, "cannot start err handler, must provide a filename.");
    cur_filename = filename;
    err_count = 0;
    handler_started = true;
}

void jet_diag_end()
{
    cur_filename = NULL;
    err_count = 0;
    handler_started = false;
}

void jet_diag_reset()
{
    err_count = 0;
}

bool jet_diag_is_started() { return handler_started; }
const char* jet_diag_get_filename() { return cur_filename; }
size_t jet_diag_get_count() { return err_count; }

// error emitters
void jet_diag_emit(jet_log_level level, 
        const char* filename, 
        uint32_t line, 
        uint32_t col, 
        const char* msg)
{
    jet_log_outputf_flc(level, filename, line, col, "%s", msg);
    report_count++;
}

void jet_diag_pushf(jet_log_level level, 
        uint32_t line, 
        uint32_t col, 
        const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char buf[JET_LOG_MSG_BUF_SIZE];
    jet_vsnprintf(buf, sizeof(buf), args);
    va_end(args);

    jet_diag_emit(level, cur_filename, line, col, buf);
}

void jet_diag_pushf_span(jet_log_level level, 
        const jet_span* span, 
        const char* fmt, ...)
{
}

void jet_diag_pushf_tok(jet_log_level level, 
        const jet_token* tok, 
        const char* fmt, ...)
{

    jet_diag_pushf_span(level, );
}

void jet_diag_pushf_node(jet_log_level level, 
        const jet_ast_node* node, 
        const char* fmt, ...)
{
}

void jet_diag_expected_token(
        const jet_token* tok, 
        jet_token_type expected)
{
}

void jet_diag_unexpected_token(const jet_token* tok)
{
}

void jet_diag_missing(
        const jet_token* anchor, 
        const char* entity)
{

}

