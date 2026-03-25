#include <jet_diag.h>
#include <jet_logger.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

typedef struct jet_diag_report
{
    const char* filename;
    uint32_t line;
    uint32_t col;
    const char* msg;
} jet_diag_report;

static bool handler_started = false;
static const char* cur_filename = NULL;


#define JET_DIAG_MAX_REPORT_COUNT (32)
static jet_diag_report reports[JET_DIAG_MAX_REPORT_COUNT];
static size_t report_count = 0; 

static void jet_diag_add_report(const char* filename, uint32_t line, uint32_t col, const char* msg)
{
    JET_ASSERTM(report_count < JET_DIAG_MAX_REPORT_COUNT, "report limit reached, haulting...");
    jet_diag_report report;
    report.filename = filename;
    report.line = line;
    report.col = col;
    report.msg = msg;
    reports[report_count] = report;
    report_count++;
}

void jet_diag_start(const char* filename)
{
    JET_ASSERTM(filename != NULL, "cannot start err handler, must provide a filename.");
    cur_filename = filename;
    report_count = 0;
    handler_started = true;
    memset(reports, 0, sizeof(reports));
}

void jet_diag_end()
{
    cur_filename = NULL;
    handler_started = false; 
    memset(reports, 0, sizeof(reports));
    report_count = 0;
}

void jet_diag_reset()
{
    report_count = 0;
    memset(reports, 0, sizeof(reports));
}

bool jet_diag_is_started() 
{ 
    return handler_started; 
}

const char* jet_diag_get_filename() 
{ 
    return cur_filename; 
}

size_t jet_diag_get_count() 
{ 
    return report_count; 
}

// error emitters
void jet_diag_emit(jet_diag_level level, 
        const char* filename, 
        uint32_t line, 
        uint32_t col, 
        const char* msg)
{
    jet_log_outputf_flc(level, filename, line, col, "%s", msg);
    jet_diag_add_report(filename, line, col, msg);
}

void jet_diag_vpushf(jet_diag_level level, 
        uint32_t line, 
        uint32_t col,
        const char* fmt, 
        va_list args)
{
    char out_buf[JET_LOG_MSG_BUF_SIZE];
    jet_vsnprintf(out_buf, sizeof(out_buf), fmt, args);
    jet_diag_emit(level, cur_filename, line, col, out_buf);
}

void jet_diag_pushf(jet_diag_level level, 
        uint32_t line, 
        uint32_t col, 
        const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    jet_diag_vpushf(level, line, col, fmt, args);
    va_end(args);
}

void jet_diag_pushf_span(jet_diag_level level, 
        const jet_span* span, 
        const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    jet_diag_vpushf(level, span->line, span->col, fmt, args);
    va_end(args);
}

void jet_diag_pushf_token(jet_diag_level level, 
        const jet_token* tok, 
        const char* fmt, ...)
{

    va_list args;
    va_start(args, fmt);
    jet_diag_vpushf(level, tok->span.line, tok->span.col, fmt, args);
    va_end(args);
}

void jet_diag_pushf_node(jet_diag_level level, 
        const jet_ast_node* node, 
        const char* fmt, ...)
{ 
    va_list args;
    va_start(args, fmt);
    jet_diag_vpushf(level, node->span.line, node->span.col, fmt, args);
    va_end(args);
}

// TOKEN ANCHORED ===
void jet_diag_expected_token(
        const jet_token* tok, 
        jet_token_type expected)
{
    jet_diag_pushf_tok(JET_DIAG_ERROR, tok, "expected token %s but got %s", 
            jet_token_type_str(expected), 
            jet_token_type_str(tok->type));
}

void jet_diag_unexpected_token(const jet_token* tok)
{
    jet_diag_pushf_tok(JET_DIAG_ERROR, tok, "unexpected token %s", jet_token_type_str(tok->type));
}

void jet_diag_missing(
        const jet_token* tok, 
        const char* name)
{
    jet_diag_pushf_token(JET_DIAG_ERROR, 
            tok, 
            "missing %s after token %s", 
            name, 
            jet_token_type_str(tok->type));
}

void jet_diag_cant_parse_interm_node(const jet_token* start_tok, const jet_token* end_tok)
{

}

void jet_daig_cant_finish_parsing(const jet_token* tok, jet_ast_node_type node_type, const char* reason)
{
    jet_diag_pushf_token(JET_DIAG_ERROR, 
            tok,
            "cannot finish parsing node of %s because %s",
            jet_ast_node_type_str(node_type),
            reason);
}

void jet_diag_cant_parse(const jet_token* tok, jet_ast_node_type node_type)
{
    jet_diag_pushf_token(JET_DIAG_ERROR, 
            tok, 
            "cannot parse node of type %s", 
            jet_ast_node_type_str(node_type));
}

void jet_diag_parser_fatal(const jet_token* tok, const char* reason)
{
    jet_diag_pushf_token(JET_DIAG_FATAL, 
            tok, 
            "fatal error occured after token %s, %s", 
            jet_token_type_str(tok->type), 
            reason);
}
