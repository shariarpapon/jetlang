#pragma once

#include <stdbool.h>

typedef enum jet_ast_node_type
{
    //root instruction container
    AST_ORIGIN, 
    AST_DOT_ACCESS,
    AST_PARAM,
    
    //Types
    AST_TYPE,
    AST_STRING,
    AST_INT,
    AST_FLOAT,
    AST_BOOL,

    //statements 
    AST_EXPR,
    AST_BLOCK, 
    AST_VAR_DECL,
    AST_RETURN,
    AST_IF, 
    AST_ELSE,
    AST_WHILE, 
    AST_FOR, 
    AST_FUNC_DEF, 
    AST_FUNC_DECL,

    //expressions
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_FUNC_CALL,
    AST_LITERAL,
    AST_VAR_REF,
    
} jet_ast_node_type;



typedef struct jet_ast_node
{
    jet_ast_node_type type;
    void*  value;

} jet_ast_node;
