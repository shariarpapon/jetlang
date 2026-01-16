#pragma once
#include <jet_list.h>

typedef enum jet_ast_node_type 
{
    AST_NODE_ROOT,       AST_NODE_IDENT,    
    AST_NODE_BLOCK,      AST_NODE_PARAM,    
    AST_NODE_DOT_ACCESS, AST_NODE_TYPE_DECL,
    AST_NODE_BINOP,      AST_NODE_UNOP,
    AST_NODE_RETURN,     AST_NODE_IF,
    AST_NODE_WHILE,      AST_NODE_FOR,    
    AST_NODE_VAR_REF,    AST_NODE_VAR_DECL,
    AST_NODE_FUNC_DEF,   AST_NODE_FUNC_DECL, 
    AST_NODE_FUNC_CALL,  AST_NODE_EXPR_STMT,
    AST_NODE_LIT,
    AST_NODE_EXPR,
} jet_ast_node_type;


typedef struct jet_ast_node
{
    jet_ast_node_type node_type;
    jet_list* child_nodes;
} jet_ast_node;

jet_ast_node* jet_ast_node_create(jet_ast_node_type node_type);
bool jet_ast_node_dispose(jet_ast_node* node);
