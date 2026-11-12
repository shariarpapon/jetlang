#include <string.h>
#include <jet_type.h>
#include <jet_ident.h>
#include <jet_logger.h>

#define JET_PTYPE_BOOL_NAME ("bool")
#define JET_PTYPE_INT_NAME ("int")
#define JET_PTYPE_FLOAT_NAME ("float")
#define JET_PTYPE_STR_NAME ("str")
#define JET_PTYPE_CHAR_NAME ("char")

#define JET_PTYPE_BOOL_SIZE (1)
#define JET_PTYPE_INT_SIZE (4)
#define JET_PTYPE_FLOAT_SIZE (4)
#define JET_PTYPE_STR_SIZE (8)
#define JET_PTYPE_CHAR_SIZE (1)

const char* jet_type_kind_str(jet_type_kind kind)
{
    switch(kind)
    {
        default:
        {
            JET_LOG_FATAL("jet type kind(%d) is not recognized", (int)kind);
            return NULL;
        }
        case JET_TYPE_INVALID: return "JET_TYPE_INVALID";
        case JET_TYPE_CUSTOM: return "JET_TYPE_CUSTOM";
        case JET_TYPE_BOOL: return "JET_TYPE_BOOL";
        case JET_TYPE_INT: return "JET_TYPE_INT";
        case JET_TYPE_FLOAT: return "JET_TYPE_FLOAT";
        case JET_TYPE_CHAR: return "JET_TYPE_CHAR";
        case JET_TYPE_STR: return "JET_TYPE_STR";
    }
}


jet_type jet_type_prim_bool()
{
    jet_type_kind kind = JET_TYPE_BOOL;
    const char* str = JET_PTYPE_BOOL_NAME;
    size_t size = JET_PTYPE_BOOL_SIZE;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type) 
    {
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    };
}

jet_type jet_type_prim_int()
{
    jet_type_kind kind = JET_TYPE_INT;
    const char* str = JET_PTYPE_INT_NAME;
    size_t size = JET_PTYPE_INT_SIZE;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type)
    {
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    }; 
}

jet_type jet_type_prim_float()
{
    jet_type_kind kind = JET_TYPE_FLOAT;
    const char* str = JET_PTYPE_FLOAT_NAME;
    size_t size = JET_PTYPE_FLOAT_SIZE;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type)
    {
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    };

}

jet_type jet_type_prim_str()
{ 
    jet_type_kind kind = JET_TYPE_STR;
    const char* str = JET_PTYPE_STR_NAME;
    size_t size = JET_PTYPE_STR_SIZE;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type)
    { 
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    };
}
jet_type jet_type_prim_char()
{
    jet_type_kind kind = JET_TYPE_CHAR;
    const char* str = JET_PTYPE_CHAR_NAME;
    size_t size = JET_PTYPE_CHAR_SIZE;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type)
    {
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    };
}
