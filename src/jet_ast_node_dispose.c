#include <jet_ast_node_dispose.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static void jet_ast_node_dispose_list(jet_list* node_list);

static void jet_ast_node_dispose_list(jet_list* node_list)
{
    if(node_list == NULL)
        return;
    size_t count = jet_list_count(node_list);
    for(size_t i = 0; i < count; i++)
    {
        jet_ast_node* node = (jet_ast_node*)jet_list_get(node_list, i);
        jet_ast_node_dispose(node);
    }
    jet_list_dispose(node_list);
}

void jet_astn_prog_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_PROG);
    assert(node->as.prog != NULL);
    jet_ast_node_dispose(node->as.prog->block);
}

void jet_astn_mem_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_MEM);
    assert(node->as.mem != NULL);
}

void jet_astn_ident_dispose(jet_ast_node* node)
{
    assert(node != NULL);
    assert(node->node_type == AST_IDENT);
    assert(node->as.ident != NULL);
}

void jet_astn_lit_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_LIT);
    assert(node->as.lit != NULL);      
    if(node->as.lit->lit_type == TOK_LIT_STR)
        if(node->as.lit->as.s)
            free((void*)node->as.lit->as.s);
}

void jet_astn_block_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_BLOCK);
    assert(node->as.block != NULL);
    jet_ast_node_dispose_list(node->as.block->node_list);
}

void jet_astn_vref_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_VAR_REF);
    assert(node->as.var_ref != NULL);
    jet_ast_node_dispose(node->as.var_ref->var_binding_ident); 
}

void jet_astn_vdecl_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_VAR_DECL);
    assert(node->as.var_decl != NULL);
    jet_ast_node_dispose(node->as.var_decl->binding_ident);
    jet_ast_node_dispose(node->as.var_decl->type_decl);
    jet_ast_node_dispose(node->as.var_decl->init_value);
}

void jet_astn_tdecl_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_TYPE_DECL);
    assert(node->as.type_decl != NULL);
}

void jet_astn_fdecl_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_FUNC_DECL);
    assert(node->as.func_decl != NULL);
    jet_ast_node_dispose(node->as.func_decl->binding_ident);
    jet_ast_node_dispose_list(node->as.func_decl->ret_type_decls);
    jet_ast_node_dispose_list(node->as.func_decl->param_var_decls);
}

void jet_astn_fdef_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_FUNC_DEF);
    assert(node->as.func_def != NULL);
    jet_ast_node_dispose(node->as.func_def->func_decl);
    jet_ast_node_dispose(node->as.func_def->block);
}

void jet_astn_fcall_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_FUNC_CALL);
    assert(node->as.func_call != NULL);
    jet_ast_node_dispose(node->as.func_call->func_binding_ident);
    jet_ast_node_dispose_list(node->as.func_call->arg_nodes);
}

void jet_astn_binop_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_BINOP);
    assert(node->as.binop != NULL);
    jet_ast_node_dispose(node->as.binop->lhs);
    jet_ast_node_dispose(node->as.binop->rhs);
}

void jet_astn_unop_dispose(jet_ast_node* node) 
{
    assert(node != NULL);
    assert(node->node_type == AST_UNOP);
    assert(node->as.unop != NULL);
    jet_ast_node_dispose(node->as.unop->term);
}
