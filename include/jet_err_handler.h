#pragma once
#include <jet_logger.h>
#include <jet_span.h>
#include <jet_ast_node.h>
#include <jet_token.h>

#include <inttypes.h>

#define JET_ERH_INV_TOK(tok) \
    jet_erh_pushf_tok(JET_LOG_LEVEL_WARNING, tok, "%s", "invalid token encountered")


void jet_erh_start(const char* filename);
void jet_erh_end();
void jet_erh_reset();

bool jet_erh_is_started(); 
const char* jet_erh_get_filename();
size_t jet_erh_get_count();

void jet_erh_pushf(jet_log_level level, const jet_span* span, const char* fmt, ...);
void jet_erh_pushf_tok(jet_log_level level, const jet_token* tok, const char*  fmt, ...);

