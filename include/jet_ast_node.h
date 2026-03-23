#pragma once
#include <jet_token.h>
#include <jet_da.h>
#include <jet_span.h>
#include <jet_type.h>

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>

typedef enum jet_ast_node_type
{
  AST_UNKNOWN = -1,
  AST_PROG = 0,  
  AST_MEM,
  AST_IDENT,
  AST_LIT,
  AST_BLOCK,
  AST_VAR_DECL,
  AST_TYPE_DECL,
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

typedef struct jet_ast_node_ident
{
    char* str;
} jet_ast_node_ident;
 
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
   } as;
} jet_ast_node_lit;

typedef struct jet_ast_node_block
{
  jet_da stmt_nid_da;
} jet_ast_node_block;

typedef struct jet_ast_node_vdecl
{    
  node_id tdecl_nid;
  node_id ident_nid;
  node_id init_value_nid;
} jet_ast_node_vdecl;

typedef struct jet_ast_node_tdecl 
{
  const char* tname;
  size_t byte_size;
  bool is_primitive;
} jet_ast_node_tdecl;

typedef struct jet_ast_node_fdecl
{
  node_id ident_nid;
  jet_da ret_tdecl_nid_da;
  jet_da param_nid_da; 
} jet_ast_node_fdecl;

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
        jet_ast_node_prog prog;
        jet_ast_node_mem mem;
        jet_ast_node_ident ident;
        jet_ast_node_lit lit;
        jet_ast_node_block block; 
        jet_ast_node_vdecl vdecl;
        jet_ast_node_tdecl tdecl;
        jet_ast_node_fdecl fdecl;
        jet_ast_node_fdef fdef;
        jet_ast_node_call call;
        jet_ast_node_binop binop;
        jet_ast_node_unop unop;
    } as;
};

bool jet_ast_node_init(jet_ast_node* node, jet_ast_node_type type, size_t start_cursor, size_t end_cursor, uint32_t line, uint32_t col);
void jet_ast_node_dispose(jet_ast_node* node);
const char* jet_ast_node_type_str(jet_ast_node_type node_type);


