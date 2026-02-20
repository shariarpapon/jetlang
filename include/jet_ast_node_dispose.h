#pragma once
#include <jet_ast_node.h>

void jet_astn_prog_dispose(jet_ast_node* node);
void jet_astn_mem_dispose(jet_ast_node* node);
void jet_astn_ident_dispose(jet_ast_node* node);
void jet_astn_lit_dispose(jet_ast_node* node);
void jet_astn_block_dispose(jet_ast_node* node);
void jet_astn_vdecl_dispose(jet_ast_node* node);
void jet_astn_tdecl_dispose(jet_ast_node* node);
void jet_astn_fdecl_dispose(jet_ast_node* node);
void jet_astn_fdef_dispose(jet_ast_node* node);
void jet_astn_call_dispose(jet_ast_node* node);
void jet_astn_binop_dispose(jet_ast_node* node);
void jet_astn_unop_dispose(jet_ast_node* node);



