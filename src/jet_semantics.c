#include <jet_semantics.h>
#include <jet_semantics_internals.h>

#include <jet_logger.h>
#include <jet_ast.h>
#include <jet_type.h>
#include <jet_da.h>
#include <jet_hs.h>

#include <jet_ast_node.h>

#define JET_SEMA_TYPE_ID_HS_CAP (32)
#define JET_SEMA_TYPE_DA_CAP (32)


// Public Interface #############################

bool jet_sema_init(jet_compilation_unit* cu)
{
    if(!cu) return false;
    
    if(!jet_sema_init_type_id_hs(&cu->type_id_hs, JET_SEMA_TYPE_ID_HS_CAP) ||
       !jet_da_init(&cu->type_da, JET_SEMA_TYPE_DA_CAP, sizeof(jet_type)) )
    {
        return false;
    }
    if(!jet_sema_register_primitive_types(cu))
        return false;
    return true;
}

bool jet_sema_resolve_types(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);
    if(!jet_sema_register_custom_types(cu))
        return false;
    return true;
}

bool jet_sema_resolve_symbols(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);
    return true;
}

const jet_type* jet_sema_get_type_by_tid(const jet_compilation_unit* cu, size_t tid)
{
    JET_ASSERT(cu != NULL);
    if(tid == JET_INVALID_TID)
        return NULL;
    size_t index = jet_sema_tid_to_index(tid);
    const jet_type* type = (const jet_type*)jet_da_get(&cu->type_da, index);
    return type;
}

const jet_type* jet_sema_get_type_by_name(const jet_compilation_unit* cu, jet_ident type_name)
{
    JET_ASSERT(cu != NULL);
    if(type_name.str == NULL || type_name.len == 0)
        return NULL;
    size_t tid = jet_sema_get_type_id(cu, type_name);
    return jet_sema_get_type_by_tid(cu, tid);
}





