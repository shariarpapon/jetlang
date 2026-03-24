#include <jet_type.h>
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
        case JET_TYPE_BOOL: return "bool";
        case JET_TYPE_INT: return "int";
        case JET_TYPE_FLOAT: return "float";
        case JET_TYPE_CHAR: return "char";
        case JET_TYPE_STR: return "str";
    }
}
