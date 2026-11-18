#include <jet_parser_internals.h>
#include <jet_logger.h>

jet_type_kind jet_parser_type_kind_from_token(const jet_token* tok)
{
    JET_ASSERT(tok != NULL);
    switch(tok->type)
    {
        default:             return JET_TYPE_INVALID;
        case TOK_IDENT:      return JET_TYPE_CUSTOM;
        case TOK_KWD_VOID:   return JET_TYPE_VOID;
        case TOK_KWD_BOOL:   return JET_TYPE_BOOL;
        case TOK_KWD_INT:    return JET_TYPE_INT;
        case TOK_KWD_FLOAT:  return JET_TYPE_FLOAT;
        case TOK_KWD_STR:    return JET_TYPE_STR;
        case TOK_KWD_CHAR:   return JET_TYPE_CHAR;
    }
}



