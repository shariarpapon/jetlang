#include <jet_parser_internals.h>
#include <jet_logger.h>

const jet_ident jet_parser_ident_from_tok(const jet_token* tok)
{
    JET_ASSERT(tok != NULL);
    return (const jet_ident)jet_ident_init(tok->lexeme, jet_span_len(tok->span));
}

jet_span jet_parser_token_range_span(const jet_token* start, const jet_token* end)
{
    JET_ASSERT(start != NULL);
    JET_ASSERT(end != NULL);
    jet_span span = 
    {
        .start = start->span.start,
        .end = end->span.end,
        .line = start->span.line,
        .col = start->span.col
    };
    return span;
}
