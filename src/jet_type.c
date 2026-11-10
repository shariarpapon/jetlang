#include <string.h>
#include <jet_type.h>
#include <jet_ident.h>
#include <jet_logger.h>

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


jet_type jet_type_bool()
{
    jet_type_kind kind = JET_TYPE_BOOL;
    const char* str = "bool";
    size_t size = 1;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type) 
    {
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    };
}

jet_type jet_type_int()
{
    jet_type_kind kind = JET_TYPE_INT;
    const char* str = "int";
    size_t size = 4;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type)
    {
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    }; 
}

jet_type jet_type_float()
{
    jet_type_kind kind = JET_TYPE_FLOAT;
    const char* str = "float";
    size_t size = 4;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type)
    {
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    };

}

jet_type jet_type_str()
{ 
    jet_type_kind kind = JET_TYPE_STR;
    const char* str = "str";
    size_t size = 1;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type)
    { 
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    };
}
jet_type jet_type_char()
{
    jet_type_kind kind = JET_TYPE_CHAR;
    const char* str = "char";
    size_t size = 1;
    jet_ident name = {.str=str,.len=strlen(str)};
    return (jet_type)
    {
        .kind=kind, 
        .type_name=name, 
        .size_bytes=size
    };
}
