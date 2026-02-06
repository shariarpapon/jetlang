#include <jet_ast_node.h>
#include <jet_ast_node_dispose.h>
#include <jet_io.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

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
        case AST_MEM:
            free(node);
            return;
        case AST_PROG:  
            jet_astn_prog_dispose(node);
            return;
        case AST_IDENT:
            jet_astn_ident_dispose(node);
            return;
        case AST_LIT:
            jet_astn_lit_dispose(node);
            return;
        case AST_BLOCK:
            jet_astn_block_dispose(node);
            return;
        case AST_VAR_REF:
            jet_astn_vref_dispose(node);
            return;
        case AST_VAR_DECL:
            jet_astn_vdecl_dispose(node);
            return;
        case AST_TYPE_DECL:
            jet_astn_tdecl_dispose(node);
            return;
        case AST_FUNC_DECL:
            jet_astn_fdecl_dispose(node);
            return;
        case AST_FUNC_DEF:
            jet_astn_fdef_dispose(node);
            return;
        case AST_FUNC_CALL:
            jet_astn_fcall_dispose(node);
            return;
        case AST_BINOP:
            jet_astn_binop_dispose(node);
            return;
        case AST_UNOP:
            jet_astn_unop_dispose(node);
            return;
    }
}


const char* jet_ast_node_to_str(jet_ast_node* node)
{
    switch(node->node_type)
    {
        default: 
            return "_ast_node_unrecognized_";
        case AST_MEM:
            return "AST_MEM";
        case AST_PROG: 
            return "AST_PROG";
        case AST_IDENT:
            return "AST_IDENT";
        case AST_LIT:
            return "AST_LIT";
        case AST_BLOCK:
            return "AST_BLOCK";
        case AST_VAR_REF:
            return "AST_VAR_REF";
        case AST_VAR_DECL:
            return "AST_VAR_DECL";
        case AST_TYPE_DECL:
            return "AST_TYPE_DECL";
        case AST_FUNC_DECL:
            return "AST_FUNC_DECL";
        case AST_FUNC_DEF:
            return "AST_FUNC_DEF";
        case AST_FUNC_CALL:
            return "AST_FUNC_CALL";
        case AST_BINOP:
            return "AST_BINOP";
        case AST_UNOP:
            return "AST_UNOP";
    }
    
}

void jet_ast_node_print(jet_ast_node* node)
{
    const char* node_str = jet_ast_node_to_str(node);
    printf("%s\n", node_str);
}


