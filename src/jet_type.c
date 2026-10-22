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
        case JET_TYPE_INVALID: return "JET_TYPE_INVALID";
        case JET_TYPE_CUSTOM: return "JET_TYPE_CUSTOM";
        case JET_TYPE_BOOL: return "JET_TYPE_BOOL";
        case JET_TYPE_INT: return "JET_TYPE_INT";
        case JET_TYPE_FLOAT: return "JET_TYPE_FLOAT";
        case JET_TYPE_CHAR: return "JET_TYPE_CHAR";
        case JET_TYPE_STR: return "JET_TYPE_STR";
    }
}
