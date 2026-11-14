#include <jet_semantics_internals.h>
#include <jet_logger.h>

void jet_sema_register_type(jet_compilation_unit* cu, jet_type type)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(jet_da_append(&cu->type_da, (const void*)&type));
    size_t type_id = jet_da_count(&cu->type_da);
    JET_ASSERT(type_id != JET_INVALID_TID);
    JET_ASSERT(jet_ident_id_kv_hs_add(&cu->type_id_hs, type.type_name, type_id));
}

void jet_sema_register_custom_types(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);    
    const jet_da* top_nid_da = (const jet_da*)&cu->ast.top_nid_da;
    size_t top_count = jet_da_count(top_nid_da);
    if(top_count == 0) 
        return;
    for(size_t i = 0; i < top_count; i++)
    {
        const jet_ast_node* node = jet_ast_get_top_node_at_index(&cu->ast, i);
        if(!jet_sema_can_node_construct_type(node))
            continue;
        if(jet_sema_has_type_name(cu, node->as.tdef.type_name))
        {
            jet_error err = jet_err_type_redef(node);
            jet_sema_push_err(cu, (const jet_error*)&err);
            continue;
        }    
        jet_type out_type = {0};
        JET_ASSERT(jet_sema_type_from_node(node, &out_type));
        jet_sema_register_type(cu, out_type);
    }
}

void jet_sema_register_primitive_types(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);
    JET_ASSERT(jet_type_primitives != NULL);
    for(size_t i = 0; i < jet_type_primitive_count; i++)
        jet_sema_register_type(cu, jet_type_primitives[i]);
}

bool jet_sema_has_type_name(const jet_compilation_unit* cu, jet_ident type_name)
{
    JET_ASSERT(cu != NULL);
    return jet_sema_get_type_id(cu, type_name) != JET_INVALID_TID;
}

size_t jet_sema_get_type_id(const jet_compilation_unit* cu, jet_ident type_name)
{
    JET_ASSERT(cu != NULL);
    size_t tid = JET_INVALID_TID; 
    jet_ident_id_kv_hs_has_key(&cu->type_id_hs, type_name, &tid);
    return tid;
}

jet_type* jet_sema_get_type_by_tid(const jet_compilation_unit* cu, size_t tid)
{
    JET_ASSERT(cu != NULL);
    if(tid == JET_INVALID_TID)
        return NULL;
    size_t index = jet_sema_tid_to_index(tid);
    jet_type* type = (jet_type*)jet_da_get(&cu->type_da, index);
    return type;
}

jet_type* jet_sema_get_type_by_name(const jet_compilation_unit* cu, jet_ident type_name)
{
    JET_ASSERT(cu != NULL);
    if(type_name.str == NULL || type_name.len == 0)
        return NULL;
    size_t tid = jet_sema_get_type_id(cu, type_name);
    return jet_sema_get_type_by_tid(cu, tid);
}

size_t jet_sema_index_to_tid(size_t index)
{
    return index + 1;
}

size_t jet_sema_tid_to_index(size_t tid)
{
    JET_ASSERT(tid != JET_INVALID_TID);
    return tid - 1;
}




