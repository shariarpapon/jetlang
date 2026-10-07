#include <jet_ast_node.h>
#include <jet_logger.h>

#include <string.h>
#include <stdio.h>

static jet_ast_node jet_astn_base(jet_ast_node_type type, jet_span span);

static jet_ast_node jet_astn_base(jet_ast_node_type type, jet_span span)
{
    jet_ast_node node;
    memset(&node, 0, sizeof(node));
    node->type = type;
    node->span = span;
    return node;
}

jet_ast_node jet_astn_prog(node_id block_nid);
jet_ast_node jet_astn_mem(size_t alloc_size);
jet_ast_node jet_astn_ident(char* str);
jet_ast_node jet_astn_lit(jet_type_kind type_kind, void* value);
jet_ast_node jet_astn_block(const jet_da* stmt_nid_da);
jet_ast_node jet_astn_vdecl(node_id tdecl_nid, node_id ident_nid, node_id init_value_nid);
jet_ast_node jet_astn_tdecl(const char* tname, size_t byte_size, bool is_primitive);
jet_ast_node jet_astn_fdecl(node_id ident_nid, const jet_da* ret_tdecl_nid_da, const jet_da* param_nid_da);
jet_ast_node jet_astn_fdef(node_id fdecl_nid, node_id block_nid);
jet_ast_node jet_astn_call(node_id callee_nid, const jet_da* arg_nid_da);
jet_ast_node jet_astn_binop(node_id lhs_nid, node_id rhs_nid, jet_token_type op_type);
jet_ast_node jet_astn_unop(node_id expr_nid, jet_token_type op_type);

void jet_ast_node_dispose(jet_ast_node* node)
{
    //dispose according to type
    if(!node) 
        return;

    switch(node->node_type)
    {
        default: 
            return;
        case AST_IDENT: 
        {
            if(node->as.ident.str)
                free((void*)node->as.ident.str);
            return;
        }
        case AST_LIT:
        {
            if(node->as.lit.tkind == JET_TYPE_STR && 
               node->as.lit.as.s)
                    free((void*)node->as.lit.as.s);
            return;
        }
        case AST_TYPE_DECL:
        {
            if(node->as.tdecl.tname)
                free((void*)node->as.tdecl.tname);
            return;
        }       
        case AST_BLOCK:
        {
            jet_da_dispose(&node->as.block.stmt_nid_da);
            return;
        }
        case AST_FUNC_DECL:
        {
            jet_da_dispose(&node->as.fdecl.ret_tdecl_nid_da);
            jet_da_dispose(&node->as.fdecl.param_nid_da);
            return;
        }
        case AST_CALL:
        {
            jet_da_dispose(&node->as.call.arg_nid_da);
            return;
        }
    }
}

const char* jet_ast_node_type_str(jet_ast_node_type node_type)
{
    switch(node_type)
    {
        default: return "## ast_node_type_undefined ##";
        case AST_UNKNOWN: return "AST_UNKNOWN";
        case AST_PROG: return "AST_PROG";
        case AST_MEM: return "AST_MEM";
        case AST_IDENT: return "AST_IDENT";
        case AST_LIT: return "AST_LIT";
        case AST_BLOCK: return "AST_BLOCK";
        case AST_VAR_DECL: return "AST_VAR_DECL";
        case AST_TYPE_DECL: return "AST_TYPE_DECL";
        case AST_FUNC_DECL: return "AST_FUNC_DECL";
        case AST_FUNC_DEF: return "AST_FUNC_DEF";
        case AST_CALL: return "AST_CALL";
        case AST_BINOP: return "AST_BINOP";
        case AST_UNOP: return "AST_UNOP";
    }    
}
