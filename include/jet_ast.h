#pragma once

#include <stdbool.h>
#include <jet_list.h>
#include <jet_lexer.h>

// {{{ decl
typedef enum   jet_ast_mem_region      jet_ast_mem_region;
typedef enum   jet_ast_node_type       jet_ast_node_type;
typedef enum   jet_ast_native_type     jet_ast_native_type;
typedef enum   jet_ast_unop_type       jet_ast_unop_type;
typedef enum   jet_ast_binop_type      jet_ast_binop_type;

typedef struct jet_ast                 jet_ast;
typedef struct jet_ast_node            jet_ast_node;
typedef struct jet_ast_node_root       jet_ast_node_root;
typedef struct jet_ast_node_ident      jet_ast_node_ident;
typedef struct jet_ast_node_expr_stmt  jet_ast_node_expr_stmt;
typedef struct jet_ast_node_block      jet_ast_node_block;
typedef struct jet_ast_node_param      jet_ast_node_param;
typedef struct jet_ast_node_lit        jet_ast_node_lit;
typedef struct jet_ast_node_dot_access jet_ast_node_dot_access;
typedef struct jet_ast_node_type_decl  jet_ast_node_type_decl;
typedef struct jet_ast_node_unop       jet_ast_node_unop;
typedef struct jet_ast_node_binop      jet_ast_node_binop;
typedef struct jet_ast_node_return     jet_ast_node_return;
typedef struct jet_ast_node_if         jet_ast_node_if;
typedef struct jet_ast_node_while      jet_ast_node_while;
typedef struct jet_ast_node_for        jet_ast_node_for;
typedef struct jet_ast_node_var_ref    jet_ast_node_var_ref;
typedef struct jet_ast_node_var_decl   jet_ast_node_var_decl;
typedef struct jet_ast_node_func_def   jet_ast_node_func_def;
typedef struct jet_ast_node_func_decl  jet_ast_node_func_decl;
typedef struct jet_ast_node_func_call  jet_ast_node_func_call;
// }}}

// {{{ def 
enum jet_ast_mem_region
{
    MEM_STACK,
    MEM_HEAP,
};

enum jet_ast_node_type 
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
};

enum jet_ast_native_type
{
   AST_NATIVE_NULL,
   AST_NATIVE_INT,
   AST_NATIVE_BOOL,
   AST_NATIVE_STR,
   AST_NATIVE_FLOAT,
}; 

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
    jet_list* token_list;
    jet_ast_node* root_node;
};

struct jet_ast_node
{
    jet_ast_node_type node_type;
    union
    {
        jet_ast_node_root*       root;
        jet_ast_node_block*      block;
        jet_ast_node_dot_access* dot_access;
        jet_ast_node_ident*      ident;
        jet_ast_node_expr_stmt*  expr_stmt;
        jet_ast_node_param*      param;
        jet_ast_node_lit*        lit;
        jet_ast_node_type_decl*  type_decl;
        jet_ast_node_binop*      binop;
        jet_ast_node_unop*       unop;
        jet_ast_node_return*     return_;
        jet_ast_node_if*         if_;
        jet_ast_node_while*      while_;
        jet_ast_node_for*        for_;
        jet_ast_node_var_ref*    var_ref;
        jet_ast_node_var_decl*   var_decl;
        jet_ast_node_func_def*   func_def;
        jet_ast_node_func_decl*  func_decl;
        jet_ast_node_func_call*  func_call;
    } as;
};

struct jet_ast_node_root
{
    jet_ast_node_block* entry_block;
}; 

struct jet_ast_node_ident
{
    const char* name;
};

struct jet_ast_node_expr_stmt
{
    jet_ast_node* expr;
};

struct jet_ast_node_param 
{
    jet_ast_node* var_decl;
}; 

struct jet_ast_node_block
{
    jet_list* stmts_list;
};

struct jet_ast_node_dot_access
{
    jet_ast_node* obj_expr;
    jet_ast_node* member_expr;
};

struct jet_ast_node_type_decl
{
    const char* type_id;
    bool is_native;
};

struct jet_ast_node_lit
{ 
    jet_ast_native_type native_type;
    jet_ast_node* type_decl;
    union
    {
        int         i;
        float       f;
        bool        b;
        const char* s;
        void*       n;
    } as;
};

struct jet_ast_node_unop
{
    jet_ast_unop_type op_type;
    jet_ast_node*     expr;
};

struct jet_ast_node_binop
{
    jet_ast_binop_type op_type;
    jet_ast_node*      lhs_expr;
    jet_ast_node*      rhs_expr;
};

struct jet_ast_node_return
{
    jet_ast_node* expr;
};

struct jet_ast_node_if
{ 
    jet_ast_node* cond_expr;
    jet_ast_node* then_block;
    jet_ast_node* else_block;
};

struct jet_ast_node_while
{ 
    jet_ast_node* cond_expr;
    jet_ast_node* body_block;
};

struct jet_ast_node_for
{
    jet_ast_node* init_expr;
    jet_ast_node* cond_expr;
    jet_ast_node* update_expr_stmt;
    jet_ast_node* body_block;
};

struct jet_ast_node_var_ref 
{
    jet_ast_node* ident;
    jet_ast_node* type_decl;
    jet_ast_mem_region mem_region;
};

struct jet_ast_node_var_decl 
{
    jet_ast_node* ident;
    jet_ast_node* type_decl; 
    jet_ast_node* def_value_expr;
    jet_ast_mem_region mem_region;
};

struct jet_ast_node_func_def 
{
    jet_ast_node* func_decl;
    jet_ast_node* block;
};

struct jet_ast_node_func_decl 
{
    jet_ast_node* ident;
    jet_ast_node* type_decl;
    jet_list* param_list;
};

struct jet_ast_node_func_call 
{
    jet_ast_node* callee;
    jet_list* arg_expr_list;
};

// }}}

jet_ast* jet_ast_create(jet_list* token_list);
bool jet_ast_dispose(jet_ast* ast);




















