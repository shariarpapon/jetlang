#include <jet_ast_node.h>
#include <jet_io.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

#define BRANCH_LINE "|_"
#define HOR_LINK "+ "

static void jet_ast_depth_print(const char* name, size_t depth);
static const char* jet_ast_node_str(jet_ast_node* node);

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
    if(node)
        free(node);
}

static const char* jet_ast_node_str(jet_ast_node* node)
{
    assert(node != NULL);
    const char* str = jet_ast_node_type_str(node->node_type);
    return str;
}

static void jet_ast_depth_print(const char* name, size_t depth)
{
    if(!name) return;
    for(size_t i = 0; i < depth; i++)
    {
        printf("\033[38;5;244m%s\033[0m", HOR_LINK);
    }
    printf("\033[38;5;49m%s\033[0m\033[0m%s\033[0m\n", BRANCH_LINE, name);
}

void jet_ast_node_darray_print(jet_darray* node_darray, size_t depth)
{
    if(!node_darray) return;
    size_t count = jet_darray_count(node_darray);
    
    jet_ast_depth_print("\033[38;5;215m[collection]\033[0m", depth);
    size_t child_depth = depth + 1;

    for(size_t i = 0; i < count; i++)
    {
        jet_ast_node* n = (jet_ast_node*)jet_darray_get(node_darray, i);
        jet_ast_node_print(n, child_depth);
    }
}

void jet_ast_node_print(jet_ast_node* node, size_t depth)
{
    if(!node) return;
    const char* depth_name = jet_ast_node_str(node);
    jet_ast_depth_print(depth_name, depth);
    
    size_t child_depth = depth + 1;
    //print child nodes
    switch(node->node_type)
    {
        default: 
            break;
        case AST_PROG:
        {
            jet_ast_node_print(node->as.prog->block, child_depth);
            break;
        }
        case AST_BLOCK:
        {
            jet_ast_node_darray_print(node->as.block->node_darray, child_depth);
            break;
        }
        case AST_VAR_DECL:
        {
            jet_ast_node_print(node->as.var_decl->ident, child_depth);
            jet_ast_node_print(node->as.var_decl->type_decl, child_depth);
            jet_ast_node_print(node->as.var_decl->init_value, child_depth);
            break;
        }
        case AST_FUNC_DECL:
        {
            jet_ast_node_print(node->as.func_decl->ident, child_depth);
            jet_ast_node_darray_print(node->as.func_decl->ret_type_darray, child_depth);
            jet_ast_node_darray_print(node->as.func_decl->param_darray, child_depth);
            break;
        }
        case AST_FUNC_DEF:
        {
            jet_ast_node_print(node->as.func_def->func_decl, child_depth);
            jet_ast_node_print(node->as.func_def->block, child_depth);
            break;
        }
        case AST_CALL:
        {
            jet_ast_node_print(node->as.call->ident, child_depth);
            jet_ast_node_darray_print(node->as.call->arg_darray, child_depth);
            break;
        }
        case AST_BINOP:
        {
            jet_ast_node_print(node->as.binop->lhs, child_depth);
            jet_ast_node_print(node->as.binop->rhs, child_depth);
            break;
        }
        case AST_UNOP:
        {
            jet_ast_node_print(node->as.unop->term, child_depth);
            break;
        }
    }
}


