#include <jet_parser_internals.h>
#include <jet_logger.h>

jet_result jet_parser_break_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    if(!jet_parser_is_break_head(p))
        return jet_parser_result_parsing_failed(AST_BREAK, jet_parser_peek_tok(p));

    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_KWD_BREAK);
    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_SEMI);
    if(!end_tok) 
        end_tok = jet_parser_peek_prev_tok(p);
    jet_span span = jet_parser_token_range_span(start_tok, end_tok);
    jet_ast_node node = jet_astn_break(span);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);
}

jet_result jet_parser_return_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    if(!jet_parser_is_return_head(p))
        return jet_parser_result_parsing_failed(AST_RETURN, jet_parser_peek_tok(p));
    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_KWD_RETURN);

    node_id value_expr_nid = JET_INVALID_NID;
    if(jet_parser_peekn_tok_type(p,0) != TOK_SEMI)
    {
        jet_result value_expr_res = jet_parser_parse_new_expr(p);
        JET_ASSERT(value_expr_res.kind == JET_RESULT_NID || value_expr_res.kind == JET_RESULT_ERROR);
        if(value_expr_res.kind == JET_RESULT_ERROR)
            return value_expr_res;
        value_expr_nid = value_expr_res.as.nid;
    }
    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_SEMI);
    if(!end_tok) 
        end_tok = jet_parser_peek_prev_tok(p);
    jet_span span = jet_parser_token_range_span(start_tok, end_tok);
    jet_ast_node node = jet_astn_return(span, value_expr_nid);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);
}

jet_result jet_parser_if_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    if(!jet_parser_is_if_head(p)) 
        goto parsing_fail;

    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_KWD_IF);

    jet_parser_expect_tok(p, TOK_LPAR);
    jet_result cond_expr_res = jet_parser_parse_new_expr(p);
    JET_ASSERT(cond_expr_res.kind == JET_RESULT_NID || cond_expr_res.kind == JET_RESULT_ERROR);
    if(cond_expr_res.kind == JET_RESULT_ERROR)
        return cond_expr_res;
    jet_parser_expect_tok(p, TOK_RPAR);
    jet_result block_res = jet_parser_block_parse(p);
    JET_ASSERT(block_res.kind == JET_RESULT_NID || block_res.kind == JET_RESULT_ERROR);
    if(block_res.kind == JET_RESULT_ERROR)
        return block_res;

    jet_result elif_res = {0};
    bool has_alt_branch = true;
    if(jet_parser_is_elseif_head(p))
    {
        jet_parser_expect_tok(p, TOK_KWD_ELSE);
        elif_res = jet_parser_if_parse(p);
    }
    else if(jet_parser_is_else_head(p))
    {
        elif_res = jet_parser_else_parse(p);
    }
    else has_alt_branch = false;

    node_id elif_nid = JET_INVALID_NID;
    if(has_alt_branch)
    {
        JET_ASSERT(elif_res.kind == JET_RESULT_NID || elif_res.kind == JET_RESULT_ERROR);
        if(elif_res.kind == JET_RESULT_ERROR)
            jet_parser_push_err(p, (const jet_error*)&elif_res.as.error);
        else elif_nid = elif_res.as.nid;
    }

    const jet_token* end_tok = jet_parser_peek_prev_tok(p);
    jet_span span = jet_parser_token_range_span(start_tok, end_tok);
    jet_ast_node node = jet_astn_if(span, cond_expr_res.as.nid, block_res.as.nid, elif_nid);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);

parsing_fail:
    return jet_parser_result_parsing_failed(AST_IF, jet_parser_peek_tok(p));
}

jet_result jet_parser_else_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    if(!jet_parser_is_else_head(p))
        goto parsing_fail;

    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_KWD_ELSE);
    jet_result block_res = jet_parser_block_parse(p);
    JET_ASSERT(block_res.kind == JET_RESULT_NID || block_res.kind == JET_RESULT_ERROR);
    if(block_res.kind == JET_RESULT_ERROR) 
        return block_res;

    const jet_token* end_tok = jet_parser_peek_prev_tok(p);
    jet_span span = jet_parser_token_range_span(start_tok, end_tok);
    jet_ast_node node = jet_astn_else(span, block_res.as.nid);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);

parsing_fail:
    return jet_parser_result_parsing_failed(AST_ELSE, jet_parser_peek_tok(p));
}

jet_result jet_parser_while_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    if(!jet_parser_is_while_head(p))
        goto parsing_fail;

    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_KWD_WHILE);

    jet_parser_expect_tok(p, TOK_LPAR);
    jet_result cond_expr_res = jet_parser_parse_new_expr(p);
    JET_ASSERT(cond_expr_res.kind == JET_RESULT_NID || cond_expr_res.kind == JET_RESULT_ERROR);
    if(cond_expr_res.kind == JET_RESULT_ERROR)
        return cond_expr_res;
    jet_parser_expect_tok(p, TOK_RPAR);
    jet_result block_res = jet_parser_block_parse(p);
    JET_ASSERT(block_res.kind == JET_RESULT_NID || block_res.kind == JET_RESULT_ERROR);
    if(block_res.kind == JET_RESULT_ERROR) 
        return block_res;

    const jet_token* end_tok = jet_parser_peek_prev_tok(p);
    jet_span span = jet_parser_token_range_span(start_tok, end_tok);
    jet_ast_node node = jet_astn_while(span, cond_expr_res.as.nid, block_res.as.nid);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);
    
parsing_fail:
    return jet_parser_result_parsing_failed(AST_WHILE, jet_parser_peek_tok(p));
}
