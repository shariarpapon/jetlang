#include <string.h>
#include <jet_type.h>
#include <jet_ident.h>
#include <jet_logger.h>

#define JET_PTYPE_VOID_NAME  ("void")
#define JET_PTYPE_BOOL_NAME  ("bool")
#define JET_PTYPE_INT_NAME   ("int")
#define JET_PTYPE_FLOAT_NAME ("float")
#define JET_PTYPE_STR_NAME   ("str")
#define JET_PTYPE_CHAR_NAME  ("char")

#define JET_PTYPE_VOID_SIZE  (0)
#define JET_PTYPE_BOOL_SIZE  (1)
#define JET_PTYPE_INT_SIZE   (4)
#define JET_PTYPE_FLOAT_SIZE (4)
#define JET_PTYPE_STR_SIZE   (8)
#define JET_PTYPE_CHAR_SIZE  (1)

const jet_type jet_type_primitives[] = 
{
    JET_STATIC_CONSUTRCT_TYPE(JET_TYPE_VOID,   JET_PTYPE_VOID_NAME,  JET_PTYPE_VOID_SIZE),
    JET_STATIC_CONSUTRCT_TYPE(JET_TYPE_BOOL,   JET_PTYPE_BOOL_NAME,  JET_PTYPE_BOOL_SIZE),
    JET_STATIC_CONSUTRCT_TYPE(JET_TYPE_INT,    JET_PTYPE_INT_NAME,   JET_PTYPE_INT_SIZE),
    JET_STATIC_CONSUTRCT_TYPE(JET_TYPE_FLOAT,  JET_PTYPE_FLOAT_NAME, JET_PTYPE_FLOAT_SIZE),
    JET_STATIC_CONSUTRCT_TYPE(JET_TYPE_STR,    JET_PTYPE_STR_NAME,   JET_PTYPE_STR_SIZE),
    JET_STATIC_CONSUTRCT_TYPE(JET_TYPE_CHAR,   JET_PTYPE_CHAR_NAME,  JET_PTYPE_CHAR_SIZE),
};
const size_t jet_type_primitive_count = sizeof(jet_type_primitives) / sizeof(jet_type_primitives[0]);

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
        case JET_TYPE_VOID: return "JET_TYPE_VOID";
        case JET_TYPE_BOOL: return "JET_TYPE_BOOL";
        case JET_TYPE_INT: return "JET_TYPE_INT";
        case JET_TYPE_FLOAT: return "JET_TYPE_FLOAT";
        case JET_TYPE_CHAR: return "JET_TYPE_CHAR";
        case JET_TYPE_STR: return "JET_TYPE_STR";
    }
}



