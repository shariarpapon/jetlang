#include <jet_parser_internals.h>
#include <jet_logger.h>

const jet_token* jet_parser_peek_tok(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_peekn_tok(p, 0);
}

const jet_token* jet_parser_peek_prev_tok(jet_parser* p)
{
    if(p->tok_cursor == 0)
        return NULL;
    return (const jet_token*)jet_da_get(p->tok_da, p->tok_cursor - 1);
}

const jet_token* jet_parser_peekn_tok(jet_parser* p, size_t n)
{ 
    JET_ASSERT(p != NULL);
    size_t count = jet_da_count(p->tok_da);

    //count > 0 is asserted at init
    if(n >= count - p->tok_cursor) 
        return (const jet_token*)jet_da_get(p->tok_da, count - 1);

    return (const jet_token*)jet_da_get(p->tok_da, p->tok_cursor + n);
}

jet_token_type jet_parser_peekn_tok_type(jet_parser* p, size_t n)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peekn_tok(p, n);
    if(tok == NULL)
        return TOK_EOF;
    return tok->type;
}

const jet_token* jet_parser_consume_tok(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    if(p->tok_cursor >= jet_da_count(p->tok_da))
        return NULL;
    p->tok_cursor++;
    return (const jet_token*)jet_da_get(p->tok_da , p->tok_cursor - 1);
}

const jet_token* jet_parser_expect_tok(jet_parser* p, jet_token_type tok_type)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peek_tok(p);
    if(!tok)
    {
        jet_error err = jet_err_expected_token(jet_parser_peek_prev_tok(p), tok_type);
        jet_parser_push_err(p, &err);
        return NULL;
    }
    else if(tok->type != tok_type)
    {
        jet_error err = jet_err_expected_token(tok, tok_type);
        jet_parser_push_err(p, (const jet_error*)&err);
        return NULL;
    }
    return jet_parser_consume_tok(p);
}

const jet_token* jet_parser_expect_tok_keep(jet_parser* p, jet_token_type tok_type)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peek_tok(p);
    if(!tok)
    {
        jet_error err = jet_err_expected_token(jet_parser_peek_prev_tok(p), tok_type);
        jet_parser_push_err(p, &err);
        return NULL;
    }
    else if(tok->type != tok_type)
    {
        jet_error err = jet_err_expected_token(tok, tok_type);
        jet_parser_push_err(p, (const jet_error*)&err);
        return NULL;
    }
    return tok;
}

