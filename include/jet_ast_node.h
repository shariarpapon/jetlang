#pragma once
#include <jet_token.h>
#include <jet_ast_node_type.h>
#include <jet_darray.h>
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
    const char* str;
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
  jet_darray* node_darray;
} jet_ast_node_block;

typedef struct jet_ast_node_var_decl
{  
  jet_ast_node* ident;
  jet_ast_node* type_decl;
  jet_ast_node* init_value;
} jet_ast_node_var_decl;

typedef struct jet_ast_node_type_decl 
{
  const char* type_name;
  size_t byte_size;
  bool is_native;
} jet_ast_node_type_decl;

typedef struct jet_ast_node_func_decl
{
  jet_ast_node* ident;
  jet_darray* ret_type_darray;
  jet_darray* param_darray; 
} jet_ast_node_func_decl;

typedef struct jet_ast_node_func_def
{
  jet_ast_node* func_decl;
  jet_ast_node* block;
} jet_ast_node_func_def; 

typedef struct jet_ast_node_call
{
  jet_ast_node* ident;
  jet_darray* arg_darray;
} jet_ast_node_call;

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
        jet_ast_node_var_decl* var_decl;
        jet_ast_node_type_decl* type_decl;
        jet_ast_node_func_decl* func_decl;
        jet_ast_node_func_def* func_def;
        jet_ast_node_call* call;
        jet_ast_node_binop* binop;
        jet_ast_node_unop* unop;
    } as;
};

jet_ast_node* jet_ast_node_create_base(jet_ast_node_type node_type);
void jet_ast_node_dispose(jet_ast_node* node);
void jet_ast_node_darray_print(jet_darray* node_darray, size_t branch);
void jet_ast_node_print(jet_ast_node* node, size_t branch);



