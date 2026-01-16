#pragma once
#include <jet_list.h>

typedef enum jet_ast_node_type 
{
    AST_ROOT,       AST_IDENT,    
    AST_BLOCK,      AST_PARAM,    
    AST_DOT_ACCESS, AST_TYPE_DECL,
    AST_BINOP,      AST_UNOP,
    AST_RETURN,     AST_IF,
    AST_WHILE,      AST_FOR,    
    AST_VAR_REF,    AST_VAR_DECL,
    AST_FUNC_DEF,   AST_FUNC_DECL, 
    AST_FUNC_CALL,  AST_EXPR_STMT,
    AST_LIT,
    AST_EXPR,
} jet_ast_node_type;


typedef struct jet_ast_node
{
    jet_ast_node_type node_type;
    jet_list* child_nodes;
} jet_ast_node;

jet_ast_node* jet_ast_node_create(jet_ast_node_type node_type);
bool jet_ast_node_dispose(jet_ast_node* node);
