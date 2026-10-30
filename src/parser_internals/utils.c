#include <jet_parser_internals.h>
#include <jet_logger.h>

//returns da of node_ids
jet_da jet_parser_parse_by_delim(jet_parser* p, 
        bool(*is_head_match)(jet_parser*), 
        jet_result (*parse_node)(jet_parser*), 
        jet_token_type delim, 
        jet_token_type end)
{
    JET_ASSERT(p != NULL);
    JET_ASSERT(is_head_match != NULL);
    JET_ASSERT(parse_node != NULL);

    jet_token_type sync_points[2];
    sync_points[0] = delim;
    sync_points[1] = end;

    jet_da res_da;
    const size_t capacity = 4;
    JET_ASSERT(jet_da_init(&res_da, capacity, sizeof(node_id)));
    while(is_head_match(p))
    {
        jet_result r = parse_node(p);
        if(r.success) 
        {
            JET_ASSERT(jet_da_append(&res_da, (const void*)&r.as.nid));
        }
        else 
        {
            jet_parser_push_err(p, &r.as.error);
            if(!jet_parser_sync_to_any(p, sync_points, ARRAY_LEN(sync_points)))
                break;
        }
        if(jet_parser_peekn_tok_type(p, 0) == delim)
        {
            jet_parser_consume_tok(p);
            continue;
        }
        else break;
    }
    jet_parser_expect_tok(p, end);
    return res_da;
}

jet_result jet_parser_register_node(jet_parser* p, const jet_ast_node* node)
{
    JET_ASSERT(p != NULL);
    JET_ASSERT(node != NULL);
    node_id nid = jet_ast_register_node(p->ast, node);
    if(nid == INVALID_NID)
    {
        JET_LOG_FATAL("failed to push node into ast, potentially overflowing/corrupted memory.");
        abort();
    }
    return jet_result_nid(nid);
}

bool jet_parser_is_sequence_match(jet_parser* p, jet_token_type seq[], size_t len)
{
    JET_ASSERT(p != NULL);
    JET_ASSERTM(seq != NULL || len == 0, "invalid usage - sequence array cannot be null with len > 0");

    for(size_t i = 0; i < len; i++)
    {
        if(seq[i] != jet_parser_peekn_tok_type(p, i))        
            return false;
    }
    return true;
}

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

uint8_t jet_parser_get_op_prec(jet_token_type op_type)
{
    switch(op_type) {
        default: 
            return PREC_NONE; 
        case TOK_INCR:
        case TOK_DECR:
        case TOK_DOT: 
            return PREC_POSTFIX;
        case TOK_NOT: 
            return PREC_PREFIX;
        case TOK_STAR:
        case TOK_SLASH:
        case TOK_MOD: 
            return PREC_MULT;
        case TOK_PLUS:
        case TOK_MINUS: 
            return PREC_ADDITIVE;
        case TOK_SHL:
        case TOK_SHR: 
            return PREC_BIT_SHIFT;
        case TOK_BAND: 
            return PREC_BAND;
        case TOK_BOR: 
            return PREC_BOR;
        case TOK_AND: 
            return PREC_AND;
        case TOK_OR: 
            return PREC_OR;
        case TOK_GTE:
        case TOK_LTE:
        case TOK_GT:
        case TOK_LT: 
            return PREC_RELATIONAL;
        case TOK_ASG:
        case TOK_PLUSEQ:
        case TOK_MINEQ:
        case TOK_MULEQ:
        case TOK_DIVEQ:
        case TOK_MODEQ:
        case TOK_XOREQ:
        case TOK_BANDEQ:
        case TOK_BOREQ: 
            return PREC_ASG;
        case TOK_POW:
            return PREC_POW;
    }
}
