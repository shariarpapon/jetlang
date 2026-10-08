#include <jet_ast_node.h>
#include <jet_logger.h>

#include <string.h>
#include <stdio.h>

static jet_ast_node jet_astn_base(jet_ast_node_type type, jet_span span);

static jet_ast_node jet_astn_base(jet_ast_node_type type, jet_span span)
{
    return (jet_ast_node){.type = type, .span = span};
}

jet_ast_node jet_astn_prog(jet_span span, node_id block_nid)
{
   jet_ast_node node = jet_astn_base(AST_PROG, span);
   node.as.prog = {.block_nid = block_nid};
   return node;
}

jet_ast_node jet_astn_mem(jet_span span, size_t alloc_size)
{
   jet_ast_node node = jet_astn_base(AST_MEM, span);
   node.as.mem = {.alloc_size = alloc_size };
   return node;
}

jet_ast_node jet_astn_ident(jet_span span, char* str)
{
   jet_ast_node node = jet_astn_base(AST_IDENT, span);
   node.as.ident = {.str = str};
   return node;
}

jet_ast_node jet_astn_lit(jet_span span, jet_type_kind tkind, void* value)
{ 
    JET_ASSERT(value != NULL);
    jet_ast_node node = jet_astn_base(AST_LIT, span);
    jet_ast_node_lit lit = {.tkind = tkind}; 
    switch(tkind)
    {
        case JET_TYPE_BOOL: lit.as.b = *((bool*)value); break;
        case JET_TYPE_INT: lit.as.i = *((int64_t*)value); break;
        case JET_TYPE_FLOAT: lit.as.f = *((double*)value); break;
        case JET_TYPE_CHAR: lit.as.c = *((char*)value); break;
        case JET_TYPE_STR: lit.as.s = *((char**)value); break;
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

jet_ast_node jet_astn_vdecl(jet_span span, node_id tdecl_nid, node_id ident_nid, node_id init_value_nid)
{
    jet_ast_node node = jet_astn_base(AST_VAR_DECL, span);
    node.as.vdecl = 
    {
        .tdecl_nid = tdecl_nid, 
        .ident_nid = ident_nid, 
        .init_value_nid = init_value_nid 
    };
    return node;
}

jet_ast_node jet_astn_tdecl(jet_span span, const char* tname, size_t byte_size, bool is_primitive)
{
    JET_ASSERT(tname != NULL);
    jet_ast_node node = jet_astn_base(AST_TYPE_DECL, span);
    node.as.tdecl = 
    {
        .tname = tname,
        .byte_size = byte_size,
        .is_primitive = is_primitive
    };
    return node;
}

jet_ast_node jet_astn_fdecl(jet_span span, node_id ident_nid, const jet_da* ret_tdecl_nid_da, const jet_da* param_nid_da)
{
    jet_ast_node node = jet_astn_base(AST_FUNC_DECL, span);

    jet_ast_node_fdecl fdecl = {.ident_nid = ident_nid};
    JET_ASSERT(jet_da_clone(&fdecl.ret_tdecl_nid_da, ret_tdecl_nid_da));
    JET_ASSERT(jet_da_clone(&fdecl.param_nid_da, param_nid_da));
    
    ndoe.as.fdecl = fdecl;
    return node;
}

jet_ast_node jet_astn_fdef(jet_span span, node_id fdecl_nid, node_id block_nid)
{
    jet_ast_node node = jet_astn_base(AST_FUNC_DEF, span);
    node.as.fdef = {.fdecl_nid = fdecl_nid, .block_nid = block_nid };
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
    node.as.binop = {.lhs_nid = lhs_nid, .rhs_nid = rhs_nid, .op_type = op_type };
    return node;
}

jet_ast_node jet_astn_unop(jet_span span, node_id expr_nid, jet_token_type op_type)
{
    jet_ast_node node = jet_astn_base(AST_UNOP, span);
    node.as.unop = {.expr_nid = expr_nid, .op_type = op_type };
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
