#include <jet_parser_internals.h>
#include <jet_logger.h>

bool jet_parser_is_expr_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    switch(jet_parser_peekn_tok_type(p, 0))
    {
        default: 
            return false;
        case TOK_IDENT:
        case TOK_LPAR:
        case TOK_LIT_INT:
        case TOK_LIT_FLOAT:
        case TOK_LIT_STR:
        case TOK_LIT_CHAR:
        case TOK_KWD_TRUE:
        case TOK_KWD_FALSE:
        case TOK_NOT:
        case TOK_MINUS:
            return true;
    }
}

bool jet_parser_is_break_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_peekn_tok_type(p,0) == TOK_KWD_BREAK;
}

bool jet_parser_is_return_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_peekn_tok_type(p,0) == TOK_KWD_RETURN;
}

bool jet_parser_is_if_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_peekn_tok_type(p, 0) == TOK_KWD_IF &&
           jet_parser_peekn_tok_type(p, 1) == TOK_LPAR;
}

bool jet_parser_is_elseif_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_peekn_tok_type(p, 0) == TOK_KWD_ELSE &&
           jet_parser_peekn_tok_type(p, 1) == TOK_KWD_IF &&
           jet_parser_peekn_tok_type(p, 2) == TOK_LPAR;
}

bool jet_parser_is_else_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_peekn_tok_type(p, 0) == TOK_KWD_ELSE &&
           jet_parser_peekn_tok_type(p, 1) == TOK_LBRC;
}

bool jet_parser_is_while_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_peekn_tok_type(p, 0) == TOK_KWD_WHILE &&
           jet_parser_peekn_tok_type(p, 1) == TOK_LPAR;
}

bool jet_parser_is_prog_head(jet_parser* p)
{
    return jet_parser_peekn_tok_type(p, 0) == TOK_KWD_PROG && 
           jet_parser_peekn_tok_type(p, 1) == TOK_LBRC;
}

bool jet_parser_is_tdef(jet_parser* p)
{
    jet_token_type seq[] = { TOK_KWD_TYPE, TOK_IDENT, TOK_LBRC};
    return jet_parser_is_sequence_match(p, seq, ARRAY_LEN(seq));
}

bool jet_parser_is_tspec(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peek_tok(p);
    return jet_parser_is_defined_type(p, tok, NULL);
}

bool jet_parser_is_vdecl_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return  jet_parser_is_tspec(p) &&
            jet_parser_peekn_tok_type(p, 1) == TOK_IDENT && 
           (jet_parser_peekn_tok_type(p, 2) == TOK_ASG || 
            jet_parser_peekn_tok_type(p, 2) == TOK_SEMI);
}

bool jet_parser_is_func_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    
    return jet_parser_is_tspec(p) &&
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

