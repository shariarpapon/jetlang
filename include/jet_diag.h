#pragma once
#include <jet_logger.h>
#include <jet_span.h>
#include <jet_ast_node.h>
#include <jet_token.h>
#include <jet_type.h>
#include <inttypes.h>

typedef jet_log_level jet_diag_level;

#define JET_DIAG_FATAL JET_LOG_LEVEL_FATAL
#define JET_DIAG_ERROR JET_LOG_LEVEL_ERROR
#define JET_DIAG_WARN JET_LOG_LEVEL_WARNING
#define JET_DIAG_INFO JET_LOG_LEVEL_INFO
#define JET_DIAG_DEBUG JET_LOG_LEVEL_DEBUG

void jet_diag_start(const char* filename);
void jet_diag_end();
void jet_diag_reset();
bool jet_diag_is_started(); 
const char* jet_diag_get_filename();
size_t jet_diag_get_count();

void jet_diag_emit(jet_diag_level level, 
        const char* filename, 
        uint32_t line, 
        uint32_t col, 
        const char* msg);

void jet_diag_pushf_span(jet_diag_level level, const jet_span* span, const char* fmt, ...);
void jet_diag_pushf_token(jet_diag_level level, const jet_token* tok, const char* fmt, ...);
void jet_diag_pushf_node(jet_diag_level level, const jet_ast_node* node, const char* fmt, ...);

void jet_diag_expected_token(const jet_token* tok, jet_token_type expected);
void jet_diag_unexpected_token(const jet_token* tok);
void jet_diag_missing(const jet_token* tok, const char* name);
 
