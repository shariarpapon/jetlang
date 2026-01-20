#pragma once

typedef enum jet_ast_node_type
{
  jet_ast_node_PROG,  
  jet_ast_node_IDENT,
  jet_ast_node_LIT,
  jet_ast_node_BLOCK,
  jet_ast_node_VAR_REF,
  jet_ast_node_VAR_DECL,
  jet_ast_node_TYPE_DECL,
  jet_ast_node_FUNC_DECL,
  jet_ast_node_FUNC_DEF,
  jet_ast_node_FUNC_CALL,
} jet_ast_node_type;
