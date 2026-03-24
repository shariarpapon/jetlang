#include <jet_diag.h>
#include <jet_logger.h>

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

static bool handler_started = false;
static const char* cur_filename = NULL;
static size_t err_count = 0;

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

void jet_diag_pushf(jet_log_level level, const jet_span* span, const char* fmt, ...) 
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
    jet_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    jet_log_outputf_flc(level, cur_filename, span->line, span->col, "%s", buf);
    err_count++;
}

void jet_diag_pushf_tok(jet_log_level level, const jet_token* tok, const char*  fmt, ...)
{
    JET_ASSERT(tok != NULL);
    JET_ASSERT(level >= JET_LOG_LEVEL_FATAL && level <= JET_LOG_LEVEL_DEBUG);

    va_list args;
    va_start(args, fmt);
    char msg_buf[JET_LOG_MSG_BUF_SIZE];
    jet_vsnprintf(msg_buf, sizeof(msg_buf), fmt, args);
    va_end(args);
    
    const char* tok_type_str = jet_token_type_str(tok->type); 
    char* tok_strdup;
    jet_token_strdup(tok, &tok_strdup);
    jet_diag_pushf(level, &tok->span, "token[%s, %s] %s", tok_type_str, tok_strdup, msg_buf);
    free(tok_strdup);
}

void jet_diag_pushf_node(jet_log_level level, const jet_ast_node* node, const char*  fmt, ...)
{
    JET_ASSERT(node != NULL);
    JET_ASSERT(level >= JET_LOG_LEVEL_FATAL && level <= JET_LOG_LEVEL_DEBUG);

    va_list args;
    va_start(args, fmt);
    char msg_buf[JET_LOG_MSG_BUF_SIZE];
    jet_vsnprintf(msg_buf, sizeof(msg_buf), fmt, args);
    va_end(args);
    
    const char* node_type_str = jet_ast_node_type_str(node->node_type); 
    jet_diag_pushf(level, &node->span, "node[%s] %s", node_type_str, msg_buf);
}

void jet_diag_pushf_cursor(jet_log_level level, size_t cursor, const char* msg)
{
    jet_log_outputf_fl(level, cur_filename, 0, "[cursor:%zu] %s", cursor, msg);
}
