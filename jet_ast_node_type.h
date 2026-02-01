#pragma once

typedef enum jet_ast_node_type
{
  AST_PROG,  
  AST_MEM,
  AST_IDENT,
  AST_LIT,
  AST_BLOCK,
  AST_VAR_REF,
  AST_VAR_DECL,
  AST_TYPE_DECL,
  AST_FUNC_DECL,
  AST_FUNC_DEF,
  AST_FUNC_CALL,
  AST_BINOP,
  AST_UNOP,
} jet_ast_node_type;
