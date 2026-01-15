#include <jet_ast.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static bool jet_ast_generate_tree(jet_ast* ast); 

static bool jet_ast_node_dispose              (jet_ast_node            *node);
static bool jet_ast_node_root_dispose         (jet_ast_node_root       *node);
static bool jet_ast_node_block_dispose        (jet_ast_node_block      *node);
static bool jet_ast_node_dot_access_dispose   (jet_ast_node_dot_access *node);
static bool jet_ast_node_binop_dispose        (jet_ast_node_binop      *node);
static bool jet_ast_node_return_dispose       (jet_ast_node_return     *node);
static bool jet_ast_node_while_dispose        (jet_ast_node_while      *node);
static bool jet_ast_node_var_ref_dispose      (jet_ast_node_var_ref    *node);
static bool jet_ast_node_func_def_dispose     (jet_ast_node_func_def   *node);
static bool jet_ast_node_func_call_dispose    (jet_ast_node_func_call  *node);
static bool jet_ast_node_lit_dispose          (jet_ast_node_lit        *node);
static bool jet_ast_node_ident_dispose        (jet_ast_node_ident      *node);
static bool jet_ast_node_param_dispose        (jet_ast_node_param      *node);
static bool jet_ast_node_type_decl_dispose    (jet_ast_node_type_decl  *node);
static bool jet_ast_node_unop_dispose         (jet_ast_node_unop       *node);
static bool jet_ast_node_if_dispose           (jet_ast_node_if         *node);
static bool jet_ast_node_for_dispose          (jet_ast_node_for        *node);
static bool jet_ast_node_var_decl_dispose     (jet_ast_node_var_decl   *node);
static bool jet_ast_node_func_decl_dispose    (jet_ast_node_func_decl  *node);
static bool jet_ast_node_expr_stmt_dispose    (jet_ast_node_expr_stmt  *node);

jet_ast* jet_ast_create(jet_list* token_list)
{
    if(!token_list)
    {
        fprintf(stderr, "error: cannot create jet_ast, invalid arg token_list.\n");
        return NULL;
    }

    if(jet_list_count(token_list) == 0)
    {
        fprintf(stderr, "error: cannot create AST, token list empty.\n");
        return NULL;
    }

    jet_ast* ast = (jet_ast*)malloc(sizeof(jet_ast));
        
    if(!ast)
    {
        fprintf(stderr, "error: cannot create jet_ast, failed to alloc mmemory.\n");
        ast = NULL;
        return NULL;
    }
    
    memset(ast, 0, sizeof(jet_ast));
   
    ast->token_list = token_list;

    if(!jet_ast_generate_tree(ast))
    {
        free(ast);
        fprintf(stderr, "error: cannot create jet_ast, failed to generate AST.\n");
        return NULL;
    }

    return ast;
}

bool jet_ast_dispose(jet_ast* ast)
{ 
    if(!ast)
    {
        fprintf(stderr, "error: cannot dispose jet_ast, invalid arg ast.\n");
        return false;
    }
    if(ast->root_node)
        jet_ast_node_dispose(ast->root_node);
    free(ast);
    return true;
}


static bool jet_ast_generate_tree(jet_ast* ast)
{
    puts("generating ast...");
    assert(ast != NULL);
    jet_ast_node* node = (jet_ast_node*)malloc(sizeof(jet_ast_node));
    assert(node != NULL);
    node->value.root = (jet_ast_node_root*)malloc(sizeof(jet_ast_node_root));
    assert(node->value.root != NULL);
    ast->root_node = node;
    puts("ast generated successfully!");
    return true;
}

static bool jet_ast_node_dispose(jet_ast_node* node)
{
    if(!node) return false;

    switch(node->node_type)
    {
        case AST_ROOT:
            return jet_ast_node_root_dispose(node->value.root);
        case AST_BLOCK:
            return jet_ast_node_block_dispose(node->value.block);
        case AST_DOT_ACCESS:
            return jet_ast_node_dot_access_dispose(node->value.dot_access);
        case AST_BINOP: 
            return jet_ast_node_binop_dispose(node->value.binop);
        case AST_RETURN:    
            return jet_ast_node_return_dispose(node->value.return_);
        case AST_WHILE:     
            return jet_ast_node_while_dispose(node->value.while_);
        case AST_VAR_REF:   
            return jet_ast_node_var_ref_dispose(node->value.var_ref);
        case AST_FUNC_DEF:  
            return jet_ast_node_func_def_dispose(node->value.func_def);
        case AST_FUNC_CALL: 
            return jet_ast_node_func_call_dispose(node->value.func_call);
        case AST_LIT:   
            return jet_ast_node_lit_dispose(node->value.lit);
        case AST_IDENT:   
            return jet_ast_node_ident_dispose(node->value.ident);
        case AST_PARAM:
            return jet_ast_node_param_dispose(node->value.param);
        case AST_TYPE_DECL:
            return jet_ast_node_type_decl_dispose(node->value.type_decl);
        case AST_UNOP:
            return jet_ast_node_unop_dispose(node->value.unop);
        case AST_IF: 
            return jet_ast_node_if_dispose(node->value.if_);
        case AST_FOR:       
            return jet_ast_node_for_dispose(node->value.for_);
        case AST_VAR_DECL:  
            return jet_ast_node_var_decl_dispose(node->value.var_decl);
        case AST_FUNC_DECL: 
            return jet_ast_node_func_decl_dispose(node->value.func_decl);
        case AST_EXPR_STMT:
            return jet_ast_node_expr_stmt_dispose(node->value.expr_stmt);
        default:
            fprintf(stderr, "wrn: cannot dispose node, unable to find type-corresponding dispose function. (ast_node_type: %d)", (int)node->node_type);
            return false;
    }
}

static bool jet_ast_node_root_dispose         (jet_ast_node_root       *node){ return false; }
static bool jet_ast_node_block_dispose        (jet_ast_node_block      *node){ return false; }
static bool jet_ast_node_dot_access_dispose   (jet_ast_node_dot_access *node){ return false; }
static bool jet_ast_node_binop_dispose        (jet_ast_node_binop      *node){ return false; }
static bool jet_ast_node_return_dispose       (jet_ast_node_return     *node){ return false; }
static bool jet_ast_node_while_dispose        (jet_ast_node_while      *node){ return false; }
static bool jet_ast_node_var_ref_dispose      (jet_ast_node_var_ref    *node){ return false; }
static bool jet_ast_node_func_def_dispose     (jet_ast_node_func_def   *node){ return false; }
static bool jet_ast_node_func_call_dispose    (jet_ast_node_func_call  *node){ return false; }
static bool jet_ast_node_lit_dispose          (jet_ast_node_lit        *node){ return false; }
static bool jet_ast_node_ident_dispose        (jet_ast_node_ident      *node){ return false; }
static bool jet_ast_node_param_dispose        (jet_ast_node_param      *node){ return false; }
static bool jet_ast_node_type_decl_dispose    (jet_ast_node_type_decl  *node){ return false; }
static bool jet_ast_node_unop_dispose         (jet_ast_node_unop       *node){ return false; }
static bool jet_ast_node_if_dispose           (jet_ast_node_if         *node){ return false; }
static bool jet_ast_node_for_dispose          (jet_ast_node_for        *node){ return false; }
static bool jet_ast_node_var_decl_dispose     (jet_ast_node_var_decl   *node){ return false; }
static bool jet_ast_node_func_decl_dispose    (jet_ast_node_func_decl  *node){ return false; }
static bool jet_ast_node_expr_stmt_dispose    (jet_ast_node_expr_stmt  *node){ return false; }





























