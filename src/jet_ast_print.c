#include <jet_ast.h>
#include <jet_io.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define BRANCH_LINE "|_"
#define HOR_LINK "+ "

static void jet_ast_depth_print(const char* name, size_t depth);

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

void jet_ast_print(jet_ast* ast)
{
    if(!ast)
    {
        fprintf(stderr, "error: cannot print, given ast is null.\n");
        return;
    }
    printf("\n=======================\n");
    printf("TOP\n");
    jet_ast_node_da_print(ast, jet_ast_get_top_nid_da(ast), 0);
    
    printf("=======================\n");
    printf("PROG\n");
    jet_ast_nid_print(ast, jet_ast_get_prog_nid(ast), 0); 
    printf("=======================\n\n");
}
void jet_ast_nid_da_print(jet_ast* ast, jet_da* nid_da, size_t depth)
{
    assert(ast != NULL);
    if(!nid_da) return;
    size_t count = jet_da_count(nid_da);
    
    jet_ast_depth_print("\033[38;5;215m[collection]\033[0m", depth);
    size_t child_depth = depth + 1;

    for(size_t i = 0; i < count; i++)
    {
        node_id* nid = (node_id*)jet_da_get(nid_da, i);
        if(!nid || *nid == INVALID_NID) continue;
        jet_ast_nid_print(ast, *nid, child_depth);
    }
}

void jet_ast_nid_print(jet_ast* ast, node_id nid, size_t depth)
{
    assert(ast != NULL);
    if(nid == INVALID_NID)
        return;

    const char* depth_name = jet_ast_node_str(node);
    jet_ast_depth_print(depth_name, depth);
    
    size_t child_depth = depth + 1;
    const jet_ast_node* node = jet_ast_node_get(ast, nid); 
    if(!node)
    {
        fpritnf(stderr, "err: could not print ast, nid=%zu corresponds to a NULL node.\n", nid);
        return;
    }

    //print child nodes
    switch(node->node_type)
    {
        default: 
            break;
        case AST_PROG:
        {
            jet_ast_nid_print(ast,node->as.prog.block_nid, child_depth);
            break;
        }
        case AST_BLOCK:
        {
            jet_ast_nid_da_print(ast,node->as.block.stmt_nid_da, child_depth);
            break;
        }
        case AST_VAR_DECL:
        {
            jet_ast_nid_print(ast,node->as.vdecl.ident_nid, child_depth);
            jet_ast_nid_print(ast,node->as.vdecl.tdecl_nid, child_depth);
            jet_ast_nid_print(ast,node->as.vdecl.init_value_nid, child_depth);
            break;
        }
        case AST_FUNC_DECL:
        {
            jet_ast_nid_print(ast,node->as.fdecl.ident_nid, child_depth);
            jet_ast_nid_da_print(ast,node->as.fdecl.ret_tdecl_nid_da, child_depth);
            jet_ast_nid_da_print(ast,node->as.fdecl.param_nid_da, child_depth);
            break;
        }
        case AST_FUNC_DEF:
        {
            jet_ast_nid_print(ast,node->as.fdef.fdecl_nid, child_depth);
            jet_ast_nid_print(ast,node->as.fdef.block_nid, child_depth);
            break;
        }
        case AST_CALL:
        {
            jet_ast_nid_print(ast,node->as.call.callee_nid, child_depth);
            jet_ast_nid_da_print(ast,node->as.call.arg_nid_da, child_depth);
            break;
        }
        case AST_BINOP:
        {
            jet_ast_nid_print(ast,node->as.binop.lhs_nid, child_depth);
            jet_ast_nid_print(ast,node->as.binop.rhs_nid, child_depth);
            break;
        }
        case AST_UNOP:
        {
            jet_ast_nid_print(ast,node->as.unop.expr_nid, child_depth);
            break;
        }
    }
}


