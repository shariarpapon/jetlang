#pragma once
#include <jet_ident.h>
#include <jet_result.h>
#include <jet_ast_node.h>

#define JET_PARSER_MAX_ERRORS (64)
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

// TRAVERSING UTILS
static const jet_token* jet_parser_peek_prev_tok(jet_parser* p);
static const jet_token* jet_parser_peek_tok(jet_parser* p);
static const jet_token* jet_parser_peekn_tok(jet_parser* p, size_t n);
static const jet_token* jet_parser_consume_tok(jet_parser* p);
static const jet_token* jet_parser_expect_tok(jet_parser* p, jet_token_type tok_type);
static jet_token_type jet_parser_peekn_tok_type(jet_parser* p, size_t n);

//returns false if token is not a defined type
static bool  jet_parser_get_type_name(jet_parser* p, const jet_token* tok, jet_ident* out_ident);
static const jet_ident jet_parser_ident_from_tok(const jet_token* tok);

// PARSING
static bool jet_parser_is_sequence_match(jet_parser* p, jet_token_type seq[], size_t len);
static bool jet_parser_is_vdecl(jet_parser* p);
static bool jet_parser_is_func_head(jet_parser* p);
static bool jet_parser_is_prog_entry(jet_parser* p);
static bool jet_parser_is_tdecl(jet_parser* p);
static bool jet_parser_is_tdef(jet_parser* p);
static bool jet_parser_is_defined_type(jet_parser* p, const jet_token* tok, const jet_ident** out_ident);

//abstract node parsing
static jet_result jet_parser_parse_toplevel(jet_parser* p);
static jet_result jet_parser_parse_expr_stmt(jet_parser* p);
static jet_result jet_parser_parse_expr(jet_parser* p, size_t min_prec);
static jet_result jet_parser_parse_primary(jet_parser* ast);
static jet_result jet_parser_parse_block_stmt(jet_parser* p);

//specific node parsing

static jet_result jet_parser_vdecl_parse(jet_parser* p);
static jet_result jet_parser_tdecl_parse(jet_parser* p);
static jet_result jet_parser_func_parse(jet_parser* p);
static jet_result jet_parser_parse_fparam(jet_parser* p);

static jet_result jet_parser_tdef_parse(jet_parser* p);
static jet_result jet_parser_prog_parse(jet_parser* p);
static jet_result jet_parser_block_parse(jet_parser* p);
static jet_result jet_parser_lit_parse(jet_parser* p);

static jet_result jet_parser_register_node(jet_parser* p, const jet_ast_node* node);
static jet_span jet_parser_token_range_span(const jet_token* start, const jet_token* end);
static jet_type_kind jet_parser_tkind_from_token(const jet_token* tok);

//returns the persistent canonical ident stored in the hashset
static const jet_ident* jet_parser_register_tdef_name(jet_parser* p, jet_ident type_name);
static void jet_parser_define_primitive_types(jet_parser* p);

//error recovery and syncrhonization 
static jet_result jet_parser_result_parsing_failed(jet_ast_node_type node_type, const jet_token* tok);
static void jet_parser_push_err(jet_parser* p, const jet_error* err);
static bool jet_parser_sync_toplevel(jet_parser* p);
static bool jet_parser_sync_block(jet_parser* p);
static bool jet_parser_sync_fparam(jet_parser* p);
