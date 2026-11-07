#include <jet_semantics.h>

#include <jet_logger.h>
#include <jet_ast.h>
#include <jet_type.h>
#include <jet_da.h>
#include <jet_hs.h>

#include <jet_ast_node.h>

static bool jet_sema_add_typedef(jet_compilation_unit* cu, jet_typedef def)
{
    JET_ASSERT(cu != NULL);
    return jet_da_append(&cu->typedef_da, (const void*)&def);
}

bool jet_sema_init()
{
    // add primitive types
    return true;
}

bool jet_sema_resolve_typedefs(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);    
    const jet_da* top_nid_da = (const jet_da*)&cu->ast.top_nid_da;
    size_t top_count = jet_da_count(top_nid_da);

    for(size_t i = 0; i < top_count; i++)
    {
        const jet_ast_node* node = jet_ast_get_node_at_index(&cu->ast, i);
    }

    return false;
}

bool jet_sema_resolve_symbols(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);




    return false;
}
