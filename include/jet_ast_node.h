#pragma once
#include <jet_token.h>
#include <jet_ast_node_type.h>
#include <jet_list.h>
#include <stdbool.h>

typedef struct jet_ast_node jet_ast_node;

// PROGRAM ENTRY POINT
typedef struct jet_ast_node_prog 
{
  jet_ast_node* block;
} jet_ast_node_prog;

typedef struct jet_ast_node_mem
{
    size_t alloc_size;
} jet_ast_node_mem;

typedef struct jet_ast_node_ident
{
  const char* start;
  size_t len;
} jet_ast_node_ident;
 
typedef struct jet_ast_node_lit
{
   /*
      -1       0       1       2       3       4
      void/v  int/i  float/f  bool/b  char/c  str/s
   */
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
  jet_list* node_list;
} jet_ast_node_block;

typedef struct jet_ast_node_var_ref 
{
  jet_ast_node* var_binding_ident;
} jet_ast_node_var_ref;

typedef struct jet_ast_node_var_decl
{  
  jet_ast_node* binding_ident;
  jet_ast_node* type_decl;
  jet_ast_node* init_value;
} jet_ast_node_var_decl;

typedef struct jet_ast_node_type_decl 
{
  jet_ast_node* type_ident;
  size_t byte_size;
  bool is_native;
} jet_ast_node_type_decl;

typedef struct jet_ast_node_func_decl
{
  jet_ast_node* binding_ident;
  jet_list* ret_type_decls;
  jet_list* param_var_decls; 
} jet_ast_node_func_decl;

typedef struct jet_ast_node_func_def
{
  jet_ast_node* func_decl;
  jet_ast_node* block;
} jet_ast_node_func_def; 

typedef struct jet_ast_node_func_call
{
  jet_ast_node* func_binding_ident;
  jet_list* arg_nodes;
} jet_ast_node_func_call;

typedef struct jet_ast_node_binop
{
    jet_ast_node* lhs;
    jet_ast_node* rhs;
    jet_token_type op_type;
} jet_ast_node_binop;

typedef struct jet_ast_node_unop
{
    jet_ast_node* term;
    jet_token_type op_type;
} jet_ast_node_unop;

// NODE BASE STRUCT
struct jet_ast_node
{
    jet_ast_node_type node_type;
    union
    {
        jet_ast_node_prog* prog;
        jet_ast_node_mem* mem;
        jet_ast_node_ident* ident;
        jet_ast_node_lit* lit;
        jet_ast_node_block* block; 
        jet_ast_node_var_ref* var_ref;
        jet_ast_node_var_decl* var_decl;
        jet_ast_node_type_decl* type_decl;
        jet_ast_node_func_decl* func_decl;
        jet_ast_node_func_def* func_def;
        jet_ast_node_func_call* func_call;
        jet_ast_node_binop* binop;
        jet_ast_node_unop* unop;
    } as;
};

jet_ast_node* jet_ast_node_create_base(jet_ast_node_type node_type);
void jet_ast_node_dispose(jet_ast_node* node);
const char* jet_ast_node_to_str(jet_ast_node* node);
void jet_ast_node_print(jet_ast_node* node);


