#pragma once
#include <jet_token.h>
#include <jet_ast_node_type.h>
#include <jet_da.h>
#include <stdbool.h>

#define INVALID_NID 0
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
    const char* str;
} jet_ast_node_ident;
 
typedef struct jet_ast_node_lit
{
   jet_token_type lit_type;
   union
   {
       void* v;
       int i;
       float f;
       bool b;
       char c;
       const char* s;
   } as;
} jet_ast_node_lit;

typedef struct jet_ast_node_block
{
  jet_da* stmt_nid_da;
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
  bool is_native;
} jet_ast_node_tdecl;

typedef struct jet_ast_node_fdecl
{
  node_id ident_nid;
  jet_da* ret_tdecl_nid_da;
  jet_da* param_nid_da; 
} jet_ast_node_fdecl;

typedef struct jet_ast_node_fdef
{
  node_id fdecl_nid;
  node_id block_nid;
} jet_ast_node_fdef; 

typedef struct jet_ast_node_call
{
  node_id callee_nid;
  jet_da* arg_nid_da;
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



