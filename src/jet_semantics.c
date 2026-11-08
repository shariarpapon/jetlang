#include <jet_semantics.h>
#include <jet_semantics_internals.h>

#include <jet_logger.h>
#include <jet_ast.h>
#include <jet_type.h>
#include <jet_da.h>
#include <jet_hs.h>

#include <jet_ast_node.h>

#define JET_SEMA_TYPE_ID_HS_CAP (32)

// Public Interface #############################

bool jet_sema_init(jet_compilation_unit* cu)
{
    if(!cu) return false;
    return jet_sema_init_type_id_hs(&cu->type_id_hs, JET_SEMA_TYPE_ID_HS_CAP);
}

bool jet_sema_resolve_types(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);    
    const jet_da* top_nid_da = (const jet_da*)&cu->ast.top_nid_da;
    size_t top_count = jet_da_count(top_nid_da);

    if(top_count == 0) 
        return true;

    for(size_t i = 0; i < top_count; i++)
    {
        const jet_ast_node* node = jet_ast_get_node_at_index(&cu->ast, i);
        jet_type out_type= {0};
        if(jet_sema_type_from_node(node, &out_type))
        {
            size_t tid = jet_sema_register_type(cu, out_type);
            JET_ASSERT(jet_sema_add_type_id(cu, out_type.type_name, tid));
        }
    }

    return true;
}

bool jet_sema_resolve_symbols(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);
    return true;
}



