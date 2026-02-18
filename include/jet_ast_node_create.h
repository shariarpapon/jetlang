#pragma once
#include <jet_ast_node.h>
#include <jet_token.h>

jet_ast_node* 
   jet_astn_prog_create(jet_ast_node* block);

jet_ast_node* 
 jet_astn_mem_create(size_t alloc_size);

jet_ast_node* 
   jet_astn_block_create(jet_list* node_list);

jet_ast_node* 
   jet_astn_ident_create(const char* str);

jet_ast_node* 
   jet_astn_lit_create(jet_token* tok);

jet_ast_node*
   jet_astn_vref_create(jet_ast_node* var_binding_ident);

jet_ast_node* 
   jet_astn_vdecl_create(
      jet_ast_node* type_decl, 
      jet_ast_node* binding_ident, 
      jet_ast_node* init_value);

jet_ast_node*
   jet_astn_tdecl_create(
      const char* type_name,
      size_t byte_size, 
      bool is_native);

jet_ast_node*
   jet_astn_fdecl_create(
      jet_ast_node* binding_ident,
      jet_list* ret_type_list,
      jet_list* param_list);

jet_ast_node*
   jet_astn_fdef_create(
      jet_ast_node* func_decl, 
      jet_ast_node* block);

jet_ast_node*
   jet_astn_fcall_create(
      jet_ast_node* func_binding_ident, 
      jet_list* arg_nodes);

jet_ast_node* 
    jet_astn_binop_create(
       jet_ast_node* lhs, 
       jet_ast_node* rhs, 
       jet_token_type op_type);

jet_ast_node* 
    jet_astn_unop_create(
        jet_ast_node* term, 
        jet_token_type op_type);





