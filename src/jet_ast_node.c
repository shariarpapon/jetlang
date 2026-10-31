#include <jet_ast_node.h>
#include <jet_logger.h>

#include <string.h>
#include <stdio.h>

static jet_ast_node jet_astn_base(jet_ast_node_type type, jet_span span);

static jet_ast_node jet_astn_base(jet_ast_node_type type, jet_span span)
{
    return (jet_ast_node){.node_type = type, .span = span};
}

jet_ast_node jet_astn_prog(jet_span span, node_id block_nid)
{
   jet_ast_node node = jet_astn_base(AST_PROG, span);
   node.as.prog = (jet_ast_node_prog){.block_nid = block_nid};
   return node;
}

jet_ast_node jet_astn_mem(jet_span span, size_t alloc_size)
{
   jet_ast_node node = jet_astn_base(AST_MEM, span);
   node.as.mem = (jet_ast_node_mem){.alloc_size = alloc_size };
   return node;
}

jet_ast_node jet_astn_lit(jet_span span, jet_type_kind tkind, const void* value)
{ 
    JET_ASSERT(value != NULL);
    jet_ast_node node = jet_astn_base(AST_LIT, span);
    jet_ast_node_lit lit = (jet_ast_node_lit){.tkind = tkind}; 
    switch(tkind)
    {
        case JET_TYPE_BOOL: 
            lit.as.b = *((const bool*)value); 
            break;
        case JET_TYPE_INT: 
            lit.as.i = *((const int64_t*)value); 
            break;
        case JET_TYPE_FLOAT: 
            lit.as.f = *((const double*)value); 
            break;
        case JET_TYPE_CHAR: 
            lit.as.c = *((const char*)value); 
            break;
        case JET_TYPE_STR: 
            lit.as.s = *((char**)value); 
            break;
        case JET_TYPE_CUSTOM:
            lit.as.ident = *((const jet_ident*)value);
            break;
    }
    node.as.lit = lit;
    return node;
}

jet_ast_node jet_astn_block(jet_span span, const jet_da* stmt_nid_da)
{
    jet_ast_node node = jet_astn_base(AST_BLOCK, span);
    jet_ast_node_block block = {0};
    JET_ASSERT(jet_da_clone(&block.stmt_nid_da, stmt_nid_da));
    node.as.block = block;
    return node;
}

jet_ast_node jet_astn_vdecl(jet_span span, const jet_ident ref_name, node_id tdecl_nid, node_id init_value_nid)
{
    jet_ast_node node = jet_astn_base(AST_VAR_DECL, span);
    node.as.vdecl = (jet_ast_node_vdecl){
        .tdecl_nid = tdecl_nid, 
        .ref_name = ref_name, 
        .init_value_nid = init_value_nid 
    };
    return node;
}

jet_ast_node jet_astn_tdecl(jet_span span, const jet_ident type_name, jet_type_kind tkind)
{
    jet_ast_node node = jet_astn_base(AST_TYPE_DECL, span);
    node.as.tdecl = 
    (jet_ast_node_tdecl){
        .tkind = tkind,
        .type_name = type_name,
    };
    return node;
}

jet_ast_node jet_astn_tdef(jet_span span, const jet_ident type_name, node_id block_nid)
{
    jet_ast_node node = jet_astn_base(AST_TYPE_DEF, span);
    node.as.tdef = (jet_ast_node_tdef){.type_name = type_name, .block_nid = block_nid};
    return node;
}

jet_ast_node jet_astn_fdecl(jet_span span, const jet_ident ref_name, node_id ret_tdecl_nid, const jet_da* param_nid_da)
{
    jet_ast_node node = jet_astn_base(AST_FUNC_DECL, span);
    jet_ast_node_fdecl fdecl = {.ref_name = ref_name, .ret_tdecl_nid = ret_tdecl_nid};
    JET_ASSERT(jet_da_clone(&fdecl.param_nid_da, param_nid_da)); 
    node.as.fdecl = fdecl;
    return node;
}

jet_ast_node jet_astn_fdef(jet_span span, node_id fdecl_nid, node_id block_nid)
{
    jet_ast_node node = jet_astn_base(AST_FUNC_DEF, span);
    node.as.fdef = (jet_ast_node_fdef){.fdecl_nid = fdecl_nid, .block_nid = block_nid };
    return node;
}

jet_ast_node jet_astn_call(jet_span span, node_id callee_nid, const jet_da* arg_nid_da)
{
    jet_ast_node node = jet_astn_base(AST_CALL, span);
    jet_ast_node_call call = {.callee_nid = callee_nid };
    JET_ASSERT(jet_da_clone(&call.arg_nid_da, arg_nid_da));
    node.as.call = call;
    return node;
}

jet_ast_node jet_astn_binop(jet_span span, node_id lhs_nid, node_id rhs_nid, jet_token_type op_type)
{
    jet_ast_node node = jet_astn_base(AST_BINOP, span);
    node.as.binop = (jet_ast_node_binop){.lhs_nid = lhs_nid, .rhs_nid = rhs_nid, .op_type = op_type };
    return node;
}

jet_ast_node jet_astn_unop(jet_span span, node_id expr_nid, jet_token_type op_type)
{
    jet_ast_node node = jet_astn_base(AST_UNOP, span);
    node.as.unop = (jet_ast_node_unop){.expr_nid = expr_nid, .op_type = op_type };
    return node;
}

void jet_ast_node_dispose(jet_ast_node* node)
{
    //dispose according to type
    if(!node) 
        return;

    switch(node->node_type)
    {
        default: 
            return;
        case AST_LIT:
        {
            if(node->as.lit.tkind == JET_TYPE_STR && 
               node->as.lit.as.s)
                    free((void*)node->as.lit.as.s);
            return;
        }
        case AST_BLOCK:
        {
            jet_da_dispose(&node->as.block.stmt_nid_da);
            return;
        }
        case AST_FUNC_DECL:
        {
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
        case AST_LIT: return "AST_LIT";
        case AST_BLOCK: return "AST_BLOCK";
        case AST_VAR_DECL: return "AST_VAR_DECL";
        case AST_TYPE_DECL: return "AST_TYPE_DECL";
        case AST_TYPE_DEF: return "AST_TYPE_DEF";
        case AST_FUNC_DECL: return "AST_FUNC_DECL";
        case AST_FUNC_DEF: return "AST_FUNC_DEF";
        case AST_CALL: return "AST_CALL";
        case AST_BINOP: return "AST_BINOP";
        case AST_UNOP: return "AST_UNOP";
    }    
}
