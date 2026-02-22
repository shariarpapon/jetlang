#include <jet_ast_node.h>
#include <jet_ast_node_dispose.h>
#include <jet_io.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#define BRANCH_LINE "|__" 
#define BRANCH_SPACE "+++"

static void jet_ast_branch_print(const char* name, size_t branch);

jet_ast_node* jet_ast_node_create_base(jet_ast_node_type node_type)
{
    jet_ast_node* node = (jet_ast_node*)malloc(sizeof(jet_ast_node));
    if(!node)
    {
        fprintf(stderr, "error: cannot create node, failed to allocate memory.\n");
        return NULL;
    }
    node->node_type = node_type;
    return node;
}

void jet_ast_node_dispose(jet_ast_node* node)
{
    if(!node) 
    {
        fprintf(stderr, "error: cannot free, provided node is NULL.\n");
        return;
    }

    switch(node->node_type)
    {
        default: 
        case AST_PROG:  
            jet_astn_prog_dispose(node);
            break;
        case AST_IDENT:
            jet_astn_ident_dispose(node);
            break;
        case AST_LIT:
            jet_astn_lit_dispose(node);
            break;
        case AST_BLOCK:
            jet_astn_block_dispose(node);
            break;
        case AST_VAR_DECL:
            jet_astn_vdecl_dispose(node);
            break;
        case AST_TYPE_DECL:
            jet_astn_tdecl_dispose(node);
            break;
        case AST_FUNC_DECL:
            jet_astn_fdecl_dispose(node);
            break;
        case AST_FUNC_DEF:
            jet_astn_fdef_dispose(node);
            break;
        case AST_CALL:
            jet_astn_call_dispose(node);
            break;
        case AST_BINOP:
            jet_astn_binop_dispose(node);
            break;
        case AST_UNOP:
            jet_astn_unop_dispose(node);
            break;
    }

    if(node)
        free(node);
}

static void jet_ast_branch_print(const char* name, size_t branch)
{
    if(!name) return;
    for(size_t i = 0; i < branch; i++)
        printf("\033[38;5;244m%s\033[0m", BRANCH_SPACE);

    printf("\033[38;5;49m%s\033[0m\033[0m%s\033[0m\n", BRANCH_LINE, name);
}

void jet_ast_node_darray_print(jet_darray* node_darray, size_t branch)
{
    if(!node_darray) return;
    size_t count = jet_darray_count(node_darray);
    
    jet_ast_branch_print("\033[38;5;215m[COLLECTION]\033[0m", branch);
    branch++;

    for(size_t i = 0; i < count; i++)
    {
        jet_ast_node* n = (jet_ast_node*)jet_darray_get(node_darray, i);
        jet_ast_node_print(n, branch);
    }
}

void jet_ast_node_print(jet_ast_node* node, size_t branch)
{
    if(!node) return;
    const char* branch_name = jet_ast_node_type_str(node->node_type);
    jet_ast_branch_print(branch_name, branch);
    branch++;

    //print child nodes
    switch(node->node_type)
    {
        default: 
            break;
        case AST_PROG:
        {
            jet_ast_node_print(node->as.prog->block, branch);
            break;
        }
        case AST_BLOCK:
        {
            jet_ast_node_darray_print(node->as.block->node_darray, branch);
            break;
        }
        case AST_VAR_DECL:
        {
            jet_ast_node_print(node->as.var_decl->ident, branch);
            jet_ast_node_print(node->as.var_decl->type_decl, branch);
            jet_ast_node_print(node->as.var_decl->init_value, branch);
            break;
        }
        case AST_FUNC_DECL:
        {
            jet_ast_node_print(node->as.func_decl->ident, branch);
            jet_ast_node_darray_print(node->as.func_decl->ret_type_darray, branch);
            jet_ast_node_darray_print(node->as.func_decl->param_darray, branch);
            break;
        }
        case AST_FUNC_DEF:
        {
            jet_ast_node_print(node->as.func_def->func_decl, branch);
            jet_ast_node_print(node->as.func_def->block, branch);
            break;
        }
        case AST_CALL:
        {
            jet_ast_node_print(node->as.call->ident, branch);
            jet_ast_node_darray_print(node->as.call->arg_darray, branch);
            break;
        }
        case AST_BINOP:
        {
            jet_ast_node_print(node->as.binop->lhs, branch);
            jet_ast_node_print(node->as.binop->rhs, branch);
            break;
        }
        case AST_UNOP:
        {
            jet_ast_node_print(node->as.unop->term, branch);
            break;
        }
    }
}


