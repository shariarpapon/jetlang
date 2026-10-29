#pragma once
#include <jet_token.h>
#include <jet_da.h>
#include <jet_span.h>
#include <jet_type.h>
#include <jet_ident.h>

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

typedef enum jet_ast_node_type
{
  AST_UNKNOWN = -1,
  AST_PROG = 0,  
  AST_MEM,
  AST_LIT,
  AST_BLOCK,
  AST_VAR_DECL,
  AST_TYPE_DECL,
  AST_TYPE_DEF,
  AST_FUNC_DECL,
  AST_FUNC_DEF,
  AST_CALL,
  AST_BINOP,
  AST_UNOP,
} jet_ast_node_type;

#define INVALID_NID (0)

typedef size_t node_id;
typedef struct jet_ast_node jet_ast_node;

// PROGRAM ENTRY POINT
typedef struct jet_ast_node_prog 
{
    node_id block_nid;
} jet_ast_node_prog;

typedef struct jet_ast_node_mem
{
    size_t alloc_size;
} jet_ast_node_mem;

typedef struct jet_ast_node_lit
{
   jet_type_kind tkind;
   union
   {
       int64_t i;
       double f;
       bool b;
       char c;
       char* s;
       jet_ident ident;
   } as;
} jet_ast_node_lit;

typedef struct jet_ast_node_block
{
  jet_da stmt_nid_da;
} jet_ast_node_block;

typedef struct jet_ast_node_vdecl
{    
  const jet_ident ref_name;
  node_id tdecl_nid;
  node_id init_value_nid;
} jet_ast_node_vdecl;

typedef struct jet_ast_node_fdecl
{
    const jet_ident ref_name;
    node_id ret_tdecl_nid;
    jet_da param_nid_da; 
} jet_ast_node_fdecl;

typedef struct jet_ast_node_tdecl 
{
  const jet_ident type_name;
  jet_type_kind tkind;
} jet_ast_node_tdecl;

typedef struct jet_ast_node_tdef
{
    const jet_ident type_name;
    node_id block_nid;
} jet_ast_node_tdef;

typedef struct jet_ast_node_fdef
{
    node_id fdecl_nid;
    node_id block_nid;
} jet_ast_node_fdef; 

typedef struct jet_ast_node_call
{
  node_id callee_nid;
  jet_da arg_nid_da;
} jet_ast_node_call;

typedef struct jet_ast_node_binop
{
    node_id lhs_nid;
    node_id rhs_nid;
    jet_token_type op_type;
} jet_ast_node_binop;

typedef struct jet_ast_node_unop
{
    node_id expr_nid;
    jet_token_type op_type;
} jet_ast_node_unop;

// NODE BASE STRUCT
struct jet_ast_node
{
    jet_ast_node_type node_type;
    jet_span span;
    union
    {
        jet_ast_node_vdecl vdecl;
        jet_ast_node_tdecl tdecl;
        jet_ast_node_fdecl fdecl;
        jet_ast_node_tdef tdef;

        jet_ast_node_prog prog;
        jet_ast_node_mem mem;
        jet_ast_node_lit lit;
        jet_ast_node_block block; 
        jet_ast_node_fdef fdef;
        jet_ast_node_call call;
        jet_ast_node_binop binop;
        jet_ast_node_unop unop;
    } as;
};

bool jet_ast_node_init(jet_ast_node* node, jet_ast_node_type type, size_t start_cursor, size_t end_cursor, uint32_t line, uint32_t col);
void jet_ast_node_dispose(jet_ast_node* node);
const char* jet_ast_node_type_str(jet_ast_node_type node_type);

jet_ast_node jet_astn_prog(jet_span span, node_id block_nid);
jet_ast_node jet_astn_mem(jet_span span, size_t alloc_size);
jet_ast_node jet_astn_lit(jet_span span, jet_type_kind tkind, const void* value);
jet_ast_node jet_astn_block(jet_span span, const jet_da* stmt_nid_da);
jet_ast_node jet_astn_vdecl(jet_span span, const jet_ident ref_name, node_id tdecl_nid, node_id init_value_nid);
jet_ast_node jet_astn_fdecl(jet_span span, const jet_ident ref_name, node_id ret_tdecl_nid, const jet_da* param_nid_da);
jet_ast_node jet_astn_tdecl(jet_span span, const jet_ident type_name, jet_type_kind tkind);
jet_ast_node jet_astn_tdef(jet_span span, const jet_ident type_name, node_id block_nid);
jet_ast_node jet_astn_fdef(jet_span span, node_id fdecl_nid, node_id block_nid);
jet_ast_node jet_astn_call(jet_span span, node_id callee_nid, const jet_da* arg_nid_da);
jet_ast_node jet_astn_binop(jet_span span, node_id lhs_nid, node_id rhs_nid, jet_token_type op_type);
jet_ast_node jet_astn_unop(jet_span span, node_id expr_nid, jet_token_type op_type);

//---

