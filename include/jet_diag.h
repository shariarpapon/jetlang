#pragma once
#include <jet_logger.h>
#include <jet_span.h>
#include <jet_ast_node.h>
#include <jet_token.h>

#include <inttypes.h>

#define JET_DIAG_INV_TOK(tok) \
    jet_diag_pushf_tok(JET_LOG_LEVEL_WARNING, tok, "%s", "invalid token encountered")

#define JET_DIAG_UNEXP_TOK(tok, exp_type) \
    jet_diag_pushf_tok(JET_LOG_LEVEL_ERROR, tok, "expected token type: %s", jet_token_type_str(exp_type))

#define JET_DIAG_UNEXP_NODE(node, exp_type) \
    jet_diag_pushf_node(JET_LOG_LEVEL_ERROR, node, "expected node type: %s", jet_ast_node_type_str((node)->node_type))

void jet_diag_start(const char* filename);
void jet_diag_end();
void jet_diag_reset();

bool jet_diag_is_started(); 
const char* jet_diag_get_filename();
size_t jet_diag_get_count();

void jet_diag_pushf(jet_log_level level, const jet_span* span, const char* fmt, ...);
void jet_diag_pushf_tok(jet_log_level level, const jet_token* tok, const char*  fmt, ...);
void jet_diag_pushf_node(jet_log_level level, const jet_ast_node* node, const char* fmt, ...);
