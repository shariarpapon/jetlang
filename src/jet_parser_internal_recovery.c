#include <jet_parser_internals.h>
#include <jet_logger.h>

jet_result jet_parser_result_parsing_failed(jet_ast_node_type node_type, const jet_token* tok)
{
    jet_error err = jet_err_parsing_failed(node_type, tok);
    return jet_result_error(&err);
}

void jet_parser_push_err(jet_parser* p, const jet_error* err)
{
    JET_ASSERT(p != NULL);
    JET_ASSERT(err != NULL);
    JET_ASSERT(p->err_da != NULL);
    JET_ASSERT(jet_da_append(p->err_da, (const void*)err));
}

bool jet_parser_sync_toplevel(jet_parser* p)
{
    while(jet_parser_peekn_tok_type(p, 0) != TOK_EOF)
    {
        switch(jet_parser_peekn_tok_type(p, 0))
        {
            default:
                jet_parser_consume_tok(p);
                break;

            case TOK_KWD_TYPE:
            case TOK_KWD_PROG:
            case TOK_KWD_NEEDS:
            case TOK_KWD_CHAR:
            case TOK_KWD_VOID:
            case TOK_KWD_INT:
            case TOK_KWD_FLOAT:
            case TOK_KWD_STR:
            case TOK_KWD_BOOL:
                return true;
        }
    }
    return false;
}

bool jet_parser_sync_block(jet_parser* p)
{
    while(jet_parser_peekn_tok_type(p, 0) != TOK_EOF)
    {
        switch(jet_parser_peekn_tok_type(p, 0))
        {
            default: 
                break;
            case TOK_RBRC:
                return true;
            case TOK_SEMI:
                jet_parser_consume_tok(p);
                return true;
        }
        jet_parser_consume_tok(p);
    }
    return false;
}

bool jet_parser_sync_fparam(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    while(jet_parser_peekn_tok_type(p, 0) != TOK_EOF)
    {
        switch(jet_parser_peekn_tok_type(p, 0))
        {
            case TOK_COMMA:
                jet_parser_consume_tok(p);
                return true;
            case TOK_RPAR:
            case TOK_SEMI:
                return true;
        } 
        jet_parser_consume_tok(p);
    }
    return false;
}
