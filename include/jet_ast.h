#pragma once
#include <stdbool.h>
#include <jet_list.h>

typedef enum   jet_ast_unop_type       jet_ast_unop_type;
typedef enum   jet_ast_binop_type      jet_ast_binop_type;
typedef struct jet_ast jet_ast;

enum jet_ast_unop_type
{
    UNOP_NEG,
    UNOP_NOT,
    UNOP_BNOT,
}; 

enum jet_ast_binop_type
{
    BINOP_ADD, BINOP_SUB, 
    BINOP_MUL, BINOP_DIV, 
    BINOP_MOD, BINOP_XOR,
    BINOP_SHL, BINOP_SHR,

    BINOP_EQ,  BINOP_NEQ, 
    BINOP_LT,  BINOP_GT, 
    BINOP_LTE, BINOP_GTE,

    BINOP_AND, BINOP_BAND,
    BINOP_OR,  BINOP_BOR, 

    BINOP_ADD_ASG, BINOP_SUB_ASG,
    BINOP_MUL_ASG, BINOP_DIV_ASG,
    BINOP_MOD_ASG, BINOP_XOR_ASG,
    BINOP_BOR_ASG, BINOP_BAND_ASG,
}; 

struct jet_ast
{
};


jet_ast* jet_ast_create(jet_list* token_list);
bool jet_ast_dispose(jet_ast* ast);




















