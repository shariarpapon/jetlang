#pragma once
#include <jet_logger.h>
#include <jet_span.h>
#include <jet_ast_node.h>
#include <jet_token.h>
#include <jet_type.h>

#include <inttypes.h>

#define JET_DIAG_INV_SYNTAX(span) \
    jet_diag_pushf(JET_LOG_LEVEL_ERROR, span, "invalid syntax")

#define JET_DIAG_INV_TOK(tok) \
    jet_diag_pushf_tok(JET_LOG_LEVEL_ERROR, tok, "invalid token")

#define JET_DIAG_UNEXP_TOK_HERE(tok) \
    jet_diag_pushf_tok(JET_LOG_LEVEL_ERROR, tok, "unexpected here")

#define JET_DIAG_UNEXP_TOK(tok, exp_type) \
    jet_diag_pushf_tok(JET_LOG_LEVEL_ERROR, tok, "expected token type: %s", jet_token_type_str(exp_type))

#define JET_DIAG_UNEXP_NODE(node, exp_type) \
    jet_diag_pushf_node(JET_LOG_LEVEL_ERROR, node, "expected node type: %s", jet_ast_node_type_str(exp_type))

#define JET_DIAG_NODE_REDEF(node) \
    jet_diag_pushf_node(JET_LOG_LEVEL_ERROR, node, "redefinition of node")

#define JET_DIAG_SYMBOL_UNDEF(node, name) \
    jet_diag_pushf_node(JET_LOG_LEVEL_ERROR, node, "undefined symbol: %s", name)

#define JET_DIAG_TYPE_MISMATCH(span, exp, got) \
    jet_diag_pushf_node(JET_LOG_LEVEL_ERROR, node, "expected type <%s> but got <%s>", jet_type_kind_str(exp), jet_type_kind_str(got))

#define JET_DIAG_CANT_PARSE_NODE(span, node_type) \
    jet_diag_pushf(JET_LOG_LEVEL_ERROR, span, "cannot parse node of type: %s", jet_ast_node_type_str(node_type))

#define JET_DIAG_CANT_PARSE(span, str) \
    jet_diag_pushf(JET_LOG_LEVEL_ERROR, span, "cannot parse: %s", str)

#define JET_DIAG_EXPECTED_HERE(span, str) \
    jet_diag_pushf(JET_LOG_LEVEL_ERROR, span, "expected: %s", str)

#define JET_DIAG_PARSING_HAULTED(tok, msg) \
    jet_diag_pushf_tok(JET_LOG_LEVEL_ERROR, tok, "parsing haulted: %s", msg)

#define JET_DIAG_CURSOR(cursor, msg) \
    jet_diag_pushf_cursor(JET_LOG_LEVEL_ERROR, cursor, msg)

void jet_diag_start(const char* filename);
void jet_diag_end();
void jet_diag_reset();

bool jet_diag_is_started(); 
const char* jet_diag_get_filename();
size_t jet_diag_get_count();

void jet_diag_pushf(jet_log_level level, const jet_span* span, const char* fmt, ...);
void jet_diag_pushf_tok(jet_log_level level, const jet_token* tok, const char*  fmt, ...);
void jet_diag_pushf_node(jet_log_level level, const jet_ast_node* node, const char* fmt, ...);
void jet_diag_pushf_cursor(jet_log_level level, size_t cursor, const char* msg);
