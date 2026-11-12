#include <stdlib.h>
#include <string.h>

#include <jet_semantics_internals.h>
#include <jet_logger.h>

static size_t jet_sema_calc_tdef_byte_size(const jet_ast_node* node);

bool jet_sema_type_from_node(const jet_ast_node* node, jet_type* out)
{
    JET_ASSERT(node != NULL);
    JET_ASSERT(out != NULL);
    if(!jet_sema_can_node_construct_type(node))
        return false;
    *out = (jet_type){.kind = JET_TYPE_CUSTOM, .type_name = node->as.tdef.type_name};
    return true;
}

bool jet_sema_can_node_construct_type(const jet_ast_node* node)
{
    JET_ASSERT(node != NULL);
    switch(node->node_type)
    {
        default:
            return false;
        case AST_TYPE_DEF:
            return true;
    }
}

static size_t jet_sema_calc_tdef_byte_size(const jet_ast_node* node)
{
    JET_ASSERT(node != NULL);
    JET_ASSERT(node->node_type == AST_TYPE_DEF);
    jet_ast_node_tdef tdef = node->as.tdef;
    return 0;
}



