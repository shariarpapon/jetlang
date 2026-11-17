#include <inttypes.h>
#include <jet_semantics_internals.h>
#include <jet_logger.h>
#include <jet_ast_node.h>

void jet_sema_register_global_symbols(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);
    const jet_ast* ast = &cu->ast;
    size_t top_count = jet_da_count(&ast->top_nid_da);
    if(top_count == 0)
        return;
    jet_symbol sym = {0};
    size_t nid;
    for(size_t i = 0; i < top_count; i++)
    {
        nid = jet_ast_get_top_nid_at_index(ast, i);
        if(!jet_sema_construct_global_symbol_from_node(cu, nid, &sym))
            continue;
        jet_sema_register_gsym(cu, &sym);
    }
}

void jet_sema_register_gsym(jet_compilation_unit* cu, const jet_symbol* sym)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(sym != NULL);
    
    if(jet_sema_has_gsym_name(cu, sym->name))
    {
        const jet_ast_node* sym_node = jet_ast_get_node_by_nid(&cu->ast, sym->node_id);
        jet_sema_err_dup_sym_name(cu, sym_node, sym->name);
        return;
    }

    JET_ASSERT(jet_da_append(&cu->global_sym_da, sym));
    size_t id = jet_da_count(&cu->global_sym_da);
    JET_ASSERT(id != JET_INVALID_SYMID);
    JET_ASSERT(jet_ident_id_kv_hs_add(&cu->global_symid_hs, sym->name, id));
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

bool jet_sema_has_gsym_name(const jet_compilation_unit* cu, jet_ident name)
{
    JET_ASSERT(cu != NULL);
    if(name.str == NULL || name.len == 0)
        return false;
     return jet_ident_id_kv_hs_has_key(&cu->global_symid_hs, name, NULL);
}

size_t jet_sema_get_gsymid_by_name(const jet_compilation_unit* cu, jet_ident name)
{
    JET_ASSERT(cu != NULL);
    size_t symid;
    if(!jet_ident_id_kv_hs_has_key(&cu->global_symid_hs, name, &symid))
        return JET_INVALID_SYMID;
    return symid; 
}

jet_symbol* jet_sema_get_gsym_by_id(const jet_compilation_unit* cu, size_t symid)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(symid != JET_INVALID_SYMID);
    size_t index = jet_sema_symid_to_index(symid);
    return (jet_symbol*)jet_da_get(&cu->global_sym_da, index);
}

jet_symbol* jet_sema_get_gsym_by_name(const jet_compilation_unit* cu, jet_ident name)
{
    JET_ASSERT(cu != NULL);
    size_t symid = jet_sema_get_gsymid_by_name(cu, name);
    if(symid == JET_INVALID_SYMID)
    {
        JET_LOG_ERROR("global symbol with name '%.*s' does not have registered id but trying to fetch it.", (int)name.len, name.str);
        return NULL;
    }
    return jet_sema_get_gsym_by_id(cu, symid);
}

bool jet_sema_generate_scope_symbols(jet_compilation_unit* cu, size_t nid, jet_da* out_da)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(out_da != NULL);
    
    const jet_ast_node* block_node = jet_ast_get_node_by_nid(&cu->ast, nid);
    JET_ASSERT(block_node != NULL);
    JET_ASSERT(block_node->node_type == AST_BLOCK);
}

bool jet_sema_construct_global_symbol_from_node(jet_compilation_unit* cu, size_t nid, jet_symbol* out_sym)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(out_sym != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    const jet_ast_node* node = jet_ast_get_node_by_nid(&cu->ast, nid);
    switch(node->node_type)
    {
        default: 
            return false;
        case AST_VAR_DECL:
            return jet_sema_construct_var_symbol(cu, nid, out_sym);
        case AST_FUNC_DEF:
            return jet_sema_construct_func_symbol(cu, nid, out_sym);
    }
}

bool jet_sema_construct_var_symbol(jet_compilation_unit* cu, size_t nid, jet_symbol* out_sym)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(out_sym != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);

    const jet_ast* ast = &cu->ast;
    const jet_ast_node* node = jet_ast_get_node_by_nid(ast, nid);
    JET_ASSERT(node != NULL);

    if(node->node_type != AST_VAR_DECL)
        return false;

    const jet_ast_node* tspec_node = jet_ast_get_node_by_nid(ast, node->as.vdecl.tspec_nid);
    JET_ASSERT(tspec_node != NULL);

    size_t type_id;
    if(!jet_sema_tspec_node_to_type_id(cu, tspec_node, &type_id))
        return false;
    
    *out_sym = jet_symbol_init_base(JET_SYM_VAR, node->as.vdecl.ref_name, type_id, nid);;
    return true;

}

bool jet_sema_construct_func_symbol(jet_compilation_unit* cu, size_t nid, jet_symbol* out_sym)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(out_sym != NULL);
    JET_ASSERT(nid != JET_INVALID_NID);
    
    const jet_ast* ast = &cu->ast;
    const jet_ast_node* node = jet_ast_get_node_by_nid(ast, nid);

    if(node->node_type != AST_FUNC_DEF)
        return false;

    const jet_ast_node* fdecl_node = jet_ast_get_node_by_nid(ast, node->as.fdef.fdecl_nid);
    JET_ASSERT(fdecl_node != NULL);

    const jet_ast_node* tspec_node = jet_ast_get_node_by_nid(ast, fdecl_node->as.fdecl.ret_tspec_nid);
    JET_ASSERT(tspec_node != NULL);

    size_t type_id;
    if(!jet_sema_tspec_node_to_type_id(cu, tspec_node, &type_id))
        return false;
    *out_sym = jet_symbol_init_base(JET_SYM_FUNC, fdecl_node->as.fdecl.ref_name, type_id, nid);
    return true;
}

