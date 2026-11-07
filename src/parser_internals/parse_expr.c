#include <jet_parser_internals.h>
#include <jet_logger.h>

static jet_result jet_parser_parse_primary_term(jet_parser* p);

jet_da jet_parser_parse_expr_seq(jet_parser* p, jet_token_type delim, jet_token_type end)
{
    JET_ASSERT(p != NULL);
    return jet_parser_parse_by_delim(p, jet_parser_parse_new_expr, delim, end);
}

jet_result jet_parser_parse_expr_stmt(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    jet_result result = jet_parser_parse_expr(p, 0);
    JET_ASSERT(result.kind == JET_RESULT_NID || result.kind == JET_RESULT_ERROR);
    if(result.kind != JET_RESULT_ERROR)
        jet_parser_expect_tok(p, TOK_SEMI);
    return result;
}

jet_result jet_parser_parse_new_expr(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_parse_expr(p, 0);
}

jet_result jet_parser_parse_expr(jet_parser* p, size_t min_prec)
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);
    jet_result primary_res = jet_parser_parse_primary_term(p);
    JET_ASSERT(primary_res.kind == JET_RESULT_NID || primary_res.kind == JET_RESULT_ERROR);
    if(primary_res.kind == JET_RESULT_ERROR)
        return primary_res;

    const jet_token* end_tok = start_tok;
    const jet_token* operator_tok;
    uint8_t operator_prec;
    jet_result rhs_res;
    jet_span span;
    jet_ast_node node;
    
    while(true)
    {
        operator_tok = jet_parser_peek_tok(p);
        operator_prec = jet_parser_get_op_prec(operator_tok->type);
        if(operator_prec == PREC_NONE || operator_prec < min_prec) 
            break;
        jet_parser_consume_tok(p);
        rhs_res = jet_parser_parse_expr(p, operator_prec + 1);
        JET_ASSERT(rhs_res.kind == JET_RESULT_NID || rhs_res.kind == JET_RESULT_ERROR);
        if(rhs_res.kind == JET_RESULT_ERROR)
            return rhs_res;
        end_tok = jet_parser_peek_prev_tok(p);
        span = jet_parser_token_range_span(start_tok, end_tok);
        node = jet_astn_binop(span, primary_res.as.nid, rhs_res.as.nid, operator_tok->type);
        primary_res = jet_parser_register_node(p, (const jet_ast_node*)&node);
    }
    return primary_res;
}

static jet_result jet_parser_parse_primary_term(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    jet_result res = {0};
    const jet_token* start_tok = jet_parser_peek_tok(p);
    switch(start_tok->type)
    {
        default:
            jet_span err_span = jet_parser_token_range_span(start_tok, start_tok);
            res.as.error = jet_err(JET_ERR_PRIMARY_EXPR_PARSING_FAILED, err_span);
            break;
        case TOK_IDENT:
        case TOK_LIT_INT:
        case TOK_LIT_FLOAT:
        case TOK_LIT_STR:
        case TOK_LIT_CHAR:
        case TOK_KWD_TRUE:
        case TOK_KWD_FALSE:
            res = jet_parser_lit_parse(p);
            break;
        case TOK_LPAR:
            jet_parser_consume_tok(p);
            res = jet_parser_parse_new_expr(p);
            JET_ASSERT(res.kind == JET_RESULT_NID || res.kind == JET_RESULT_ERROR);
            if(res.kind == JET_RESULT_NID)
                jet_parser_expect_tok(p, TOK_RPAR);
            break;
        case TOK_MINUS:
        case TOK_NOT:
            res = jet_parser_unop_parse(p);
            JET_ASSERT(res.kind == JET_RESULT_NID || res.kind == JET_RESULT_ERROR);
            break;
    }    
    if(res.kind == JET_RESULT_ERROR) 
        return res;
    if(jet_parser_is_call_head(p))
        res = jet_parser_call_parse(p, res.as.nid);
    return res;
}

jet_result jet_parser_unop_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);

    if(!jet_parser_is_unop_head(p))
        goto parsing_fail;

    const jet_token* start_tok = jet_parser_peek_tok(p);
    uint8_t prec = jet_parser_get_op_prec(start_tok->type);
    if(prec == PREC_NONE)
        goto parsing_fail;

    jet_parser_consume_tok(p);
    jet_result expr_res = jet_parser_parse_expr(p, PREC_PREFIX);
    JET_ASSERT(expr_res.kind == JET_RESULT_NID || expr_res.kind == JET_RESULT_ERROR);
    if(expr_res.kind == JET_RESULT_ERROR)
        return expr_res;
    const jet_token* end_tok = jet_parser_peek_prev_tok(p);
    jet_span span = jet_parser_token_range_span(start_tok, end_tok);
    jet_ast_node node = jet_astn_unop(span, expr_res.as.nid, start_tok->type);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);


parsing_fail:
    return jet_parser_result_parsing_failed(AST_UNOP, jet_parser_peek_tok(p));
}













