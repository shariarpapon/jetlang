#pragma once
#include <jet_logger.h>
#include <jet_span.h>
#include <jet_ast_node.h>
#include <jet_token.h>

#include <inttypes.h>

#define JET_ERR_HEADER_FMT "[%s:%"PRIu32":%"PRIu32"]"
#define JET_ERR_HEADER_ARGS(filename, span) filename, span->line, span->col

void jet_err_handler_push(
        const char* filename, 
        const jet_span* span, 
        const char* msg);

void jet_err_handler_fmt(
        const char* filename, 
        const jet_span* span, 
        const char* fmt, ...);
