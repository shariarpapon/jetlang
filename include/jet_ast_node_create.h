#pragma once
#include <jet_ast_node.h>
#include <jet_token.h>

jet_ast_node* 
   jet_astn_prog_create(jet_ast_node* block);

jet_ast_node* 
 jet_astn_mem_create(size_t alloc_size);

jet_ast_node* 
   jet_astn_block_create(jet_darray* node_darray);

jet_ast_node* 
   jet_astn_ident_create(const char* str);

jet_ast_node* 
   jet_astn_lit_create(jet_token* tok);

jet_ast_node* 
   jet_astn_vdecl_create(
      jet_ast_node* type_decl, 
      jet_ast_node* ident, 
      jet_ast_node* init_value);

jet_ast_node*
   jet_astn_tdecl_create(
      const char* type_name,
      size_t byte_size, 
      bool is_native);

jet_ast_node*
   jet_astn_fdecl_create(
      jet_ast_node* ident,
      jet_darray* ret_type_darray,
      jet_darray* param_darray);

jet_ast_node*
   jet_astn_fdef_create(
      jet_ast_node* func_decl, 
      jet_ast_node* block);

jet_ast_node*
   jet_astn_call_create(
      jet_ast_node* ident, 
      jet_darray* arg_nodes);

jet_ast_node* 
    jet_astn_binop_create(
       jet_ast_node* lhs, 
       jet_ast_node* rhs, 
       jet_token_type op_type);

jet_ast_node* 
    jet_astn_unop_create(
        jet_ast_node* term, 
        jet_token_type op_type);





