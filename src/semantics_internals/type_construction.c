#include <jet_semantics_internals.h>

bool jet_sema_type_from_node(const jet_ast_node* node, jet_type* out)
{
    if(node == NULL || out == NULL)
        return false;

    switch(node->node_type)
    {
        default: 
            return false;
        case AST_TYPE_DEF:
            *out = (jet_type){.kind = JET_TYPE_CUSTOM, .type_name = node->as.tdef.type_name};
            return true;
    }
}


