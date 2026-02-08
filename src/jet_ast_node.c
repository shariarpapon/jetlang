#include <jet_ast_node.h>
#include <jet_ast_node_dispose.h>
#include <jet_io.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

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
        case AST_VAR_REF:
            jet_astn_vref_dispose(node);
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
        case AST_FUNC_CALL:
            jet_astn_fcall_dispose(node);
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
    const char* branch_line = "|__";
    const char* branch_space = "+++";
    for(size_t i = 0; i < branch; i++)
        printf("%s", branch_space);
    printf("%s%s\n", branch_line, name);
}

void jet_ast_node_list_print(jet_list* node_list, size_t branch)
{
    if(!node_list) return;
    size_t count = jet_list_count(node_list);
    for(size_t i = 0; i < count; i++)
    {
        jet_ast_node* n = (jet_ast_node*)jet_list_get(node_list, i);
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
            jet_ast_node_list_print(node->as.block->node_list, branch);
            break;
        }
        case AST_VAR_REF:
        {
            jet_ast_node_print(node->as.var_ref->var_binding_ident, branch);
            break;
        }
        case AST_VAR_DECL:
        {
            jet_ast_node_print(node->as.var_decl->binding_ident, branch);
            jet_ast_node_print(node->as.var_decl->type_decl, branch);
            jet_ast_node_print(node->as.var_decl->init_value, branch);
            break;
        }
        case AST_TYPE_DECL:
        {
            jet_ast_node_print(node->as.type_decl->type_ident, branch);
            break;
        }
        case AST_FUNC_DECL:
        {
            jet_ast_node_print(node->as.func_decl->binding_ident, branch);
            jet_ast_node_list_print(node->as.func_decl->ret_type_decls, branch);
            jet_ast_node_list_print(node->as.func_decl->param_var_decls, branch);
            break;
        }
        case AST_FUNC_DEF:
        {
            jet_ast_node_print(node->as.func_def->func_decl, branch);
            jet_ast_node_print(node->as.func_def->block, branch);
            break;
        }
        case AST_FUNC_CALL:
        {
            jet_ast_node_print(node->as.func_call->func_binding_ident, branch);
            jet_ast_node_list_print(node->as.func_call->arg_nodes, branch);
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


