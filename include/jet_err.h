#pragma once
#include <jet_logger.h>
#include <jet_span.h>
#include <jet_ast_node.h>
#include <jet_token.h>

#include <inttypes.h>

typedef enum jet_err_source
{
    JET_COMPILATION_UNIT = 0,
    JET_LEXER = 1,
    JET_PARSER = 2,
    JET_TYPE_CHECKER = 3
} jet_err_source;

void jet_err_emit(jet_err_source err_src, const char* filename, const jet_span* span, const char* msg)
{ 
    const char* src_strs[] = {"COMPILATION_UNIT", "LEXER", "PARSER", "TYPE_CHECKER" };
    const char* src_str = src_strs[err_src]; 
    JET_ERROR("[%s] [%s:"PRIu32":"PRIu32"] %s", src_str, filename, span->line, span->col, msg);
}
