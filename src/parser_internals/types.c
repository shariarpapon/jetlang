#include <string.h>

#include <jet_parser_internals.h>
#include <jet_logger.h>
#include <jet_hs.h>

void jet_parser_add_primitive_type_names(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    JET_ASSERT(p->type_name_hs != NULL);
    JET_ASSERT(jet_type_primitives != NULL);

    const jet_ident* stored;
    jet_type prim_type;
    for(size_t i = 0; i < jet_type_primitive_count; i++)
    {
        prim_type = jet_type_primitives[i];
        stored = jet_parser_register_type_name(p, prim_type.type_name);
        JET_ASSERT(stored != NULL);
    }
}

const jet_ident* jet_parser_register_type_name(jet_parser* p, jet_ident type_name)
{
    JET_ASSERT(p != NULL);
    const jet_ident* stored = jet_ident_hashset_add(p->type_name_hs, type_name);
    if(stored == NULL)
    {
        JET_LOG_ERROR("failed to store ident to type_name_hs.");
        return NULL;
    }
    return stored;
}

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

bool jet_parser_get_type_name(jet_parser* p, const jet_token* tok, jet_ident* out_ident)
{
    JET_ASSERT(p != NULL);
    JET_ASSERT(tok != NULL);

    const jet_ident* stored;
    if(!jet_parser_is_defined_type(p, tok, &stored))
        goto fail;
    if(out_ident)
        *out_ident = *stored; 
    return true;
fail:
    JET_LOG_ERROR("failed to get type name, token '%s' is not a defined type.", jet_token_type_str(tok->type));
    return false;
}

bool jet_parser_is_defined_type(jet_parser* p, const jet_token* tok, const jet_ident** out_ident)
{
    JET_ASSERT(p != NULL);
    JET_ASSERT(tok != NULL);
    const jet_ident ident = jet_parser_ident_from_tok(tok);
    const void* stored;
    if(jet_hs_contains(p->type_name_hs, &ident, &stored))
    {
        if(out_ident != NULL)
            *out_ident = (const jet_ident*)stored;
        return true;
    }
    return false;
}



