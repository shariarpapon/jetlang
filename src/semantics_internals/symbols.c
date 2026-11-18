#include <inttypes.h>
#include <jet_semantics_internals.h>
#include <jet_logger.h>
#include <jet_ast_node.h>

#define JET_SEMA_INIT_SCOPE_SYM_CAP (8)

void jet_sema_register_global_symbols(jet_semantics* sema)
{
    JET_ASSERT(sema != NULL);
    const jet_ast* ast = sema->ast;
    size_t top_count = jet_da_count(&ast->top_nid_da);
    if(top_count == 0)
        return;
    jet_symbol sym = {0};
    size_t nid;
    for(size_t i = 0; i < top_count; i++)
    {
        nid = jet_ast_get_top_nid_at_index(ast, i);
        if(!jet_sema_construct_global_symbol_from_node(sema, nid, &sym))
            continue;
        jet_sema_register_gsym(sema, &sym);
    }
}

void jet_sema_register_gsym(jet_semantics* sema, const jet_symbol* sym)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(sym != NULL);
    
    if(jet_sema_has_gsym_name(sema, sym->name))
    {
        const jet_ast_node* sym_node = jet_ast_get_node_by_nid(sema->ast, sym->node_id);
        jet_sema_err_dup_sym_name(sema, sym_node, sym->name);
        return;
    }

    JET_ASSERT(jet_da_append(sema->global_sym_da, sym));
    size_t id = jet_da_count(sema->global_sym_da);
    JET_ASSERT(id != JET_INVALID_SYMID);
    JET_ASSERT(jet_ident_id_kv_hs_add(sema->global_symid_hs, sym->name, id));
}

size_t jet_sema_index_to_symid(size_t index)
{
    return index + 1;
}

size_t jet_sema_symid_to_index(size_t symid)
{
    JET_ASSERT(symid != JET_INVALID_SYMID);
    return symid - 1;
}

bool jet_sema_has_gsym_name(const jet_semantics* sema, jet_ident name)
{
    JET_ASSERT(sema != NULL);
    if(name.str == NULL || name.len == 0)
        return false;
     return jet_ident_id_kv_hs_has_key(sema->global_symid_hs, name, NULL);
}

size_t jet_sema_get_gsymid_by_name(const jet_semantics* sema, jet_ident name)
{
    JET_ASSERT(sema != NULL);
    size_t symid;
    if(!jet_ident_id_kv_hs_has_key(sema->global_symid_hs, name, &symid))
        return JET_INVALID_SYMID;
    return symid; 
}

jet_symbol* jet_sema_get_gsym_by_id(const jet_semantics* sema, size_t symid)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(symid != JET_INVALID_SYMID);
    size_t index = jet_sema_symid_to_index(symid);
    return (jet_symbol*)jet_da_get(sema->global_sym_da, index);
}

jet_symbol* jet_sema_get_gsym_by_name(const jet_semantics* sema, jet_ident name)
{
    JET_ASSERT(sema != NULL);
    size_t symid = jet_sema_get_gsymid_by_name(sema, name);
    if(symid == JET_INVALID_SYMID)
    {
        JET_LOG_ERROR("global symbol with name '%.*s' does not have registered id but trying to fetch it.", (int)name.len, name.str);
        return NULL;
    }
    return jet_sema_get_gsym_by_id(sema, symid);
}

//takes the uninitialized jet_da
void jet_sema_generate_scope_symbols(jet_semantics* sema, size_t nid, jet_da* out_da)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(out_da != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    
    const jet_ast_node* block_node = jet_ast_get_node_by_nid(sema->ast, nid);
    JET_ASSERT(block_node != NULL);
    JET_ASSERT(block_node->node_type == AST_BLOCK);

    JET_ASSERT(jet_da_init(out_da, JET_SEMA_INIT_SCOPE_SYM_CAP, sizeof(jet_symbol)));
    const jet_da* stmt_nid_da = &block_node->as.block.stmt_nid_da;
    size_t stmt_nid;
    jet_symbol sym = {0};
    for(size_t i = 0; i < jet_da_count(stmt_nid_da); i++)
    {
        stmt_nid = *((const size_t*)jet_da_get(stmt_nid_da, i));
        if(jet_sema_construct_scope_symbol_from_node(sema, stmt_nid, &sym))
            JET_ASSERT(jet_da_append(out_da, &sym));
    }
}

bool jet_sema_construct_scope_symbol_from_node(jet_semantics* sema, size_t nid, jet_symbol* out_sym)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(out_sym != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);

    const jet_ast_node* node = jet_ast_get_node_by_nid(sema->ast, nid);
    JET_ASSERT(node != NULL);

    switch(node->node_type)
    {
        default: 
            return false;
        case AST_VAR_DECL:
            return jet_sema_construct_var_symbol(sema, nid, out_sym);
    }
}

bool jet_sema_construct_global_symbol_from_node(jet_semantics* sema, size_t nid, jet_symbol* out_sym)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(out_sym != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    const jet_ast_node* node = jet_ast_get_node_by_nid(sema->ast, nid);
    JET_ASSERT(node != NULL);
    switch(node->node_type)
    {
        default: 
            return false;
        case AST_VAR_DECL:
            return jet_sema_construct_var_symbol(sema, nid, out_sym);
        case AST_FUNC_DEF:
            return jet_sema_construct_func_def_symbol(sema, nid, out_sym);
        case AST_FUNC_DECL:
            return jet_sema_construct_func_decl_symbol(sema, nid, out_sym);
    }
}

bool jet_sema_construct_func_decl_symbol(jet_semantics* sema, size_t nid, jet_symbol* out_sym)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(out_sym != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    const jet_ast_node* node = jet_ast_get_node_by_nid(sema->ast, nid);
    JET_ASSERT(node != NULL);

    if(node->node_type != AST_FUNC_DECL)
        return false;
    
    const jet_ast_node* tspec_node = jet_ast_get_node_by_nid(sema->ast, node->as.fdecl.ret_tspec_nid);
    JET_ASSERT(tspec_node != NULL);

    size_t type_id;
    if(!jet_sema_tspec_node_to_type_id(sema, tspec_node, &type_id))
        return false;
    *out_sym = jet_symbol_init_base(JET_SYM_FUNC, node->as.fdecl.ref_name, type_id, nid);
    return true;
}

bool jet_sema_construct_func_def_symbol(jet_semantics* sema, size_t nid, jet_symbol* out_sym)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(out_sym != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    const jet_ast_node* node = jet_ast_get_node_by_nid(sema->ast, nid);
    if(node->node_type != AST_FUNC_DEF)
        return false;
    return jet_sema_construct_func_decl_symbol(sema, node->as.fdef.fdecl_nid, out_sym);
}

bool jet_sema_construct_var_symbol(jet_semantics* sema, size_t nid, jet_symbol* out_sym)
{
    JET_ASSERT(sema != NULL);
    JET_ASSERT(out_sym != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);

    const jet_ast_node* node = jet_ast_get_node_by_nid(sema->ast, nid);
    JET_ASSERT(node != NULL);

    if(node->node_type != AST_VAR_DECL)
        return false;

    const jet_ast_node* tspec_node = jet_ast_get_node_by_nid(sema->ast, node->as.vdecl.tspec_nid);
    JET_ASSERT(tspec_node != NULL);

    size_t type_id;
    if(!jet_sema_tspec_node_to_type_id(sema, tspec_node, &type_id))
        return false;
    
    *out_sym = jet_symbol_init_base(JET_SYM_VAR, node->as.vdecl.ref_name, type_id, nid);;
    return true;

}

