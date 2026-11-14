#include <inttypes.h>

#include <jet_semantics_internals.h>
#include <jet_logger.h>

void jet_sema_register_global_symbols(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);
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
