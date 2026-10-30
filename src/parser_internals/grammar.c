#include <jet_parser_internals.h>
#include <jet_logger.h>

bool jet_parser_is_prog_entry(jet_parser* p)
{
    return jet_parser_peekn_tok_type(p, 0) == TOK_KWD_PROG;
}

bool jet_parser_is_tdef(jet_parser* p)
{
    jet_token_type seq[] = { TOK_KWD_TYPE, TOK_IDENT, TOK_LBRC };
    return jet_parser_is_sequence_match(p, seq, ARRAY_LEN(seq));
}

bool jet_parser_is_tdecl(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peek_tok(p);
    return jet_parser_is_defined_type(p, tok, NULL);
}

bool jet_parser_is_vdecl(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return  jet_parser_is_tdecl(p) &&
            jet_parser_peekn_tok_type(p, 1) == TOK_IDENT && 
           (jet_parser_peekn_tok_type(p, 2) == TOK_ASG || 
            jet_parser_peekn_tok_type(p, 2) == TOK_SEMI);
}

bool jet_parser_is_func_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    
    return jet_parser_is_tdecl(p) &&
           jet_parser_peekn_tok_type(p, 1) == TOK_IDENT &&
           jet_parser_peekn_tok_type(p, 2) == TOK_LPAR;
}

bool jet_parser_is_unop_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    switch(jet_parser_peekn_tok_type(p, 0))
    {
        default:
            return false;
        case TOK_MINUS:
        case TOK_NOT:
            return true;
    }
}

bool jet_parser_is_call_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_peekn_tok_type(p, 0) == TOK_LPAR;
}

