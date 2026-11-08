#include <jet_semantics.h>
#include <jet_semantics_internals.h>

#include <jet_logger.h>
#include <jet_ast.h>
#include <jet_type.h>
#include <jet_da.h>
#include <jet_hs.h>

#include <jet_ast_node.h>

// Public Interface #############################

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

    if(top_count == 0) 
        return true;

    for(size_t i = 0; i < top_count; i++)
    {
        const jet_ast_node* node = jet_ast_get_node_at_index(&cu->ast, i);
        jet_typedef out_typedef= {0};
        if(jet_sema_typedef_from_node(node, &out_typedef))
            jet_sema_register_typedef(cu, out_typedef);
    }

    return true;
}

bool jet_sema_resolve_symbols(jet_compilation_unit* cu)
{
    JET_ASSERT(cu != NULL);
    return true;
}



// Semantics Internals ##############################


// @Construction
bool jet_sema_typedef_from_node(const jet_ast_node* node, jet_typedef* out)
{
    if(node == NULL || out == NULL)
        return false;

    switch(node->node_type)
    {
        default: 
            return false;
    }
}


