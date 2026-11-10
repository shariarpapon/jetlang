#include <stdlib.h>
#include <string.h>
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


jet_type* jet_sema_get_primitive_types(size_t* out_len)
{
    if(out_len == NULL)
        return NULL;

    jet_ident bool_name = {.str="bool",sizeof("bool")};
    jet_ident int_name = {.str="int", sizeof("int")};
    jet_ident float_name = {.str="float", sizeof("float")};
    jet_ident str_name = {.str="str", sizeof("str")};
    jet_ident char_name = {.str="char", sizeof("char")};

    jet_type types[] = 
    { 
        {.kind = JET_TYPE_BOOL,  .type_name = bool_name, .size_bytes=1},
        {.kind = JET_TYPE_INT,   .type_name = int_name,  .size_bytes=4},
        {.kind = JET_TYPE_FLOAT, .type_name = float_name,.size_bytes=4},
        {.kind = JET_TYPE_STR,   .type_name = str_name,  .size_bytes=1},
        {.kind = JET_TYPE_CHAR,  .type_name = char_name, .size_bytes=1},
    };
    
    size_t size = sizeof(types);
    jet_type* out_types = malloc(size);
    if(out_types == NULL)
        return NULL;
    memcpy(out_types, types, size);
    *out_len  = size / sizeof(types[0]);
    return out_types;
}


