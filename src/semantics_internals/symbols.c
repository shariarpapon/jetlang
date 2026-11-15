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

    const jet_ast_node* node;
    const jet_ast_node* tspec_node;
    size_t type_id;
    jet_symbol_kind sym_kind;
    jet_ident ref_name;
    jet_symbol resolved_symbol;
    for(size_t i = 0; i < top_count; i++)
    {
        node = jet_ast_get_top_node_at_index(ast, i);
        switch(node->node_type)
        {
            default: 
                continue;
            case AST_VAR_DECL:
            {
                tspec_node = jet_ast_get_node_by_nid(ast, node->as.vdecl.tspec_nid);
                JET_ASSERT(tspec_node != NULL);
                type_id = jet_sema_get_type_id(cu, tspec_node->as.tspec.type_name);
                if(type_id == JET_INVALID_TID)
                {
                    jet_sema_err_undefined_type(cu, tspec_node);
                    continue;
                }
                ref_name = node->as.vdecl.ref_name;
                sym_kind = JET_SYM_VAR;
                break;
            }
            case AST_FUNC_DEF:
            {
                const jet_ast_node* fdecl_node = jet_ast_get_node_by_nid(ast, node->as.fdef.fdecl_nid);
                JET_ASSERT(fdecl_node != NULL);
                tspec_node = jet_ast_get_node_by_nid(ast, fdecl_node->as.fdecl.ret_tspec_nid);
                JET_ASSERT(tspec_node != NULL);
                type_id = jet_sema_get_type_id(cu, tspec_node->as.tspec.type_name);
                
                if(type_id == JET_INVALID_TID)
                {
                    jet_sema_err_undefined_type(cu, tspec_node);
                    continue;
                }
                ref_name = fdecl_node->as.fdecl.ref_name;
                sym_kind = JET_SYM_FUNC;
                break;
            }
        }

        if(jet_sema_has_gsym_name(cu, ref_name))
        {
            jet_sema_err_dup_sym_name(cu, node, ref_name);
            continue;
        }
        resolved_symbol = jet_symbol_init_base(sym_kind, ref_name, type_id);
        jet_sema_register_gsym(cu, &resolved_symbol);
    }
}

void jet_sema_register_gsym(jet_compilation_unit* cu, const jet_symbol* sym)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(sym != NULL);
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
