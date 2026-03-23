#pragma once
#include <jet_logger.h>
#include <jet_span.h>
#include <jet_ast_node.h>
#include <jet_token.h>

#include <inttypes.h>

#define JET_ERR(span, fmt, ...) \
    jet_erh_pushf(span, fmt, ##__VA_ARGS__)

#define JET_ERR_INV_TOK(tok) \
    jet_erh_inv_tok(tok)

void jet_erh_start(const char* filename);
void jet_erh_end();
void jet_erh_reset();

bool jet_erh_is_started(); 
const char* jet_erh_get_filename();
size_t jet_erh_get_count();

void jet_erh_push(const jet_span* span, const char* msg);
void jet_erh_pushf(const jet_span* span, const char* fmt, ...);

void jet_erh_inv_tok(const jet_token* tok);



