#pragma once

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
  AST_FUNC_CALL,
  AST_BINOP,
  AST_UNOP,
} jet_ast_node_type;

const char* jet_ast_node_type_str(jet_ast_node_type node_type);
