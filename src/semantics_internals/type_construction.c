#include <stdlib.h>
#include <string.h>

#include <jet_semantics_internals.h>
#include <jet_logger.h>

static size_t jet_sema_calc_tdef_byte_size(const jet_ast_node* node);

bool jet_sema_type_from_node(const jet_ast_node* node, jet_type* out)
{
    JET_ASSERT(node != NULL);
    JET_ASSERT(out != NULL);
    if(node->node_type != AST_TYPE_DEF)
        return false;
    *out = (jet_type){.kind = JET_TYPE_CUSTOM, .type_name = node->as.tdef.type_name};
    return true;
}

jet_type* jet_sema_get_primitive_types(size_t* out_len)
{
    if(out_len == NULL)
        return NULL;

    jet_type types[] = 
    { 
        jet_type_prim_bool(),
        jet_type_prim_int(),
        jet_type_prim_float(),
        jet_type_prim_str(),
        jet_type_prim_char()
    };
    
    size_t size = sizeof(types);
    jet_type* out_types = malloc(size);
    if(out_types == NULL)
        return NULL;
    memcpy(out_types, types, size);
    *out_len  = size / sizeof(types[0]);
    return out_types;
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



