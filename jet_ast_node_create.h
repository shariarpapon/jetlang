#pragma once
#include <jet_ast_node.h>

jet_ast_node_prog* 
   jet_astn_prog_create(jet_ast_node* block);

jet_ast_node_block* 
   jet_astn_block_create(jet_list* node_list);
jet_ast_node_ident* 
   jet_astn_ident_create(const char* ident);

jet_ast_node_lit* 
   jet_astn_lit_create(int lit_type, void* value);


jet_ast_node_var_ref* 
   jet_astn_vref_create(jet_ast_node* var_binding_ident);

jet_ast_node_var_decl* 
   jet_astn_vdecl_create(
      jet_ast_node* binding_ident, 
      jet_ast_node* type_decl, 
      jet_ast_node* init_value);

jet_ast_node_type_decl*
   jet_astn_tdecl_create(
      jet_ast_node* type_ident, 
      size_t byte_size, 
      bool is_native);

jet_ast_node_func_decl* 
   jet_astn_fdecl_create(
      jet_ast_node* binding_ident,
      jet_list* ret_type_decls,
      jet_list* param_var_decls);

jet_ast_node_func_def* 
   jet_astn_fdef_create(
      jet_ast_node* func_decl, 
      jet_ast_node* block);

jet_ast_node_func_call* 
   jet_astn_fcall_create(
      jet_ast_node* func_binding_ident, 
      jet_list* arg_nodes);












