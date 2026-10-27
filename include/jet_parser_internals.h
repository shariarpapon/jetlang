#pragma once
#include <inttypes.h>

#include <jet_parser.h>
#include <jet_ident.h>
#include <jet_result.h>
#include <jet_ast_node.h>
#include <jet_type.h>

#define JET_PARSER_MAX_ERRORS (64)
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

//operator precedence
#define PREC_NONE (0)
#define PREC_ASG (1)   // =  +=  -=  *=  /=  %=  &=  |=  ^=
#define PREC_OR (2)   // ||
#define PREC_AND (3)   // &&
#define PREC_BOR (4)   // |
#define PREC_XOR (5)   // ^
#define PREC_BAND (6)   // &
#define PREC_EQUALITY (7)   // ==  !=
#define PREC_RELATIONAL (8)   // <  <=  >  >=
#define PREC_BIT_SHIFT (9)   // <<  >>
#define PREC_ADDITIVE (10)  // +  -
#define PREC_MULT (11) // *  /  %
#define PREC_POW (12)  // **      (right-assoc)
#define PREC_PREFIX (13)  // !  ++a  --a
#define PREC_POSTFIX (14) // a++ b--  

//### traversal
const jet_token* jet_parser_peek_prev_tok(jet_parser* p);
const jet_token* jet_parser_peek_tok(jet_parser* p);
const jet_token* jet_parser_peekn_tok(jet_parser* p, size_t n);
const jet_token* jet_parser_consume_tok(jet_parser* p);
const jet_token* jet_parser_expect_tok(jet_parser* p, jet_token_type tok_type);
jet_token_type jet_parser_peekn_tok_type(jet_parser* p, size_t n);

//### grammar
bool jet_parser_is_sequence_match(jet_parser* p, jet_token_type seq[], size_t len);
bool jet_parser_is_vdecl(jet_parser* p);
bool jet_parser_is_func_head(jet_parser* p);
bool jet_parser_is_prog_entry(jet_parser* p);
bool jet_parser_is_tdecl(jet_parser* p);
bool jet_parser_is_tdef(jet_parser* p);

//### types
const jet_ident* jet_parser_register_type_name(jet_parser* p, jet_ident type_name);
bool jet_parser_is_defined_type(jet_parser* p, const jet_token* tok, const jet_ident** out_ident);
bool  jet_parser_get_type_name(jet_parser* p, const jet_token* tok, jet_ident* out_ident);
void jet_parser_define_primitive_types(jet_parser* p);
jet_type_kind jet_parser_type_kind_from_token(const jet_token* tok);

//### parse
jet_result jet_parser_parse_toplevel(jet_parser* p);
jet_result jet_parser_parse_expr_stmt(jet_parser* p);
jet_result jet_parser_parse_expr(jet_parser* p, size_t min_prec);
jet_result jet_parser_parse_primary(jet_parser* ast);
jet_result jet_parser_parse_block_stmt(jet_parser* p);

jet_result jet_parser_parse_fparam(jet_parser* p);
jet_result jet_parser_vdecl_parse(jet_parser* p);
jet_result jet_parser_tdecl_parse(jet_parser* p);
jet_result jet_parser_func_parse(jet_parser* p);
jet_result jet_parser_tdef_parse(jet_parser* p);
jet_result jet_parser_prog_parse(jet_parser* p);
jet_result jet_parser_block_parse(jet_parser* p);
jet_result jet_parser_lit_parse(jet_parser* p);

//### ast
jet_result jet_parser_register_node(jet_parser* p, const jet_ast_node* node);

//### recovery
jet_result jet_parser_result_parsing_failed(jet_ast_node_type node_type, const jet_token* tok);
void jet_parser_push_err(jet_parser* p, const jet_error* err);
bool jet_parser_sync_toplevel(jet_parser* p);
bool jet_parser_sync_block(jet_parser* p);
bool jet_parser_sync_fparam(jet_parser* p);

//### utils
jet_span jet_parser_token_range_span(const jet_token* start, const jet_token* end);
const jet_ident jet_parser_ident_from_tok(const jet_token* tok);
uint8_t jet_parser_get_op_prec(jet_token_type op_type);






