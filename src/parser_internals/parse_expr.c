#include <jet_parser_internals.h>
#include <jet_logger.h>

static jet_result jet_parser_parse_primary(jet_parser* p);

jet_result jet_parser_parse_expr_stmt(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    jet_result result = jet_parser_parse_expr(p, 0);
    if(result.success)
        jet_parser_expect_tok(p, TOK_SEMI);
    return result;
}

jet_result jet_parser_parse_expr(jet_parser* p, size_t min_prec)
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);
    node_id lhs_nid = jet_parser_parse_primary(p);
    if(lhs_nid == INVALID_NID)
    {
        return INVALID_NID;
    }
    while(jet_parser_peek_tok(p) != NULL)
    {
        const jet_token* op_tok = jet_parser_peek_tok(p);
        if(op_tok == NULL)
            break;
        uint8_t op_prec = jet_parser_get_op_prec(op_tok->type);
        if(op_prec == PREC_NONE)
            break;
        if(op_prec < min_prec)
            break;
        jet_parser_consume_tok(p);
        node_id rhs_nid = jet_parser_parse_expr(p, op_prec + 1);        
        if(rhs_nid == INVALID_NID)
        {
            return INVALID_NID;
        }
        
        const jet_token* end_tok = jet_parser_peek_prev_tok(p);

        jet_ast_node_binop binop;
        binop.lhs_nid = lhs_nid;
        binop.rhs_nid = rhs_nid;
        binop.op_type = op_tok->type;
        
        jet_ast_node node;
        JET_ASSERT(jet_ast_node_init(&node, AST_BINOP, start_tok->span.start, end_tok->span.end, start_tok->span.line, start_tok->span.col));
        node.as.binop = binop;

        lhs_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
    }
    return lhs_nid;
}

jet_result jet_parser_unop_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);
    uint8_t prec = jet_parser_get_op_prec(start_tok->type);
    if(prec == PREC_NONE)
        return jet_parser_result_parsing_failed(AST_UNOP, start_tok);
    jet_parser_consume_tok(p);
    jet_result expr_res = jet_parser_parse_expr(p, prec + 1);
    if(!expr_res.success)
        return expr_res;
    const jet_token* end_tok = jet_parser_peek_prev_tok(p);
    jet_span span = jet_parser_token_range_span(start_tok, end_tok);
    jet_ast_node node = jet_astn_unop(span, expr_res.as.nid, start_tok->type);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);
}

static jet_result jet_parser_parse_primary(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* cur_tok = jet_parser_peek_tok(p);
    const jet_token* start_tok = cur_tok;
    const jet_token* end_tok = cur_tok;
    if(cur_tok == NULL) 
        return INVALID_NID;

    node_id out_nid = INVALID_NID;

    switch(cur_tok->type)
    {
        default:
        {
            return INVALID_NID;
        }
        case TOK_KWD_NULL:
        case TOK_KWD_TRUE:
        case TOK_KWD_FALSE:
        case TOK_LIT_INT:
        case TOK_LIT_FLOAT:
        case TOK_LIT_CHAR:
        case TOK_LIT_STR:
        case TOK_IDENT:
        {
            out_nid = jet_parser_lit_parse(p);
            break;
        }
        case TOK_LPAR:
        {
            jet_parser_consume_tok(p);
            out_nid = jet_parser_parse_expr(p, 0);
            cur_tok = jet_parser_peek_tok(p);
            jet_parser_expect_tok(p, TOK_RPAR);
            break;
        }
        case TOK_NOT:
        case TOK_MINUS:
        {
            jet_parser_consume_tok(p);
            node_id rhs_nid = jet_parser_parse_expr(p, PREC_PREFIX);
            if(rhs_nid == INVALID_NID)
            {
                out_nid = INVALID_NID;
                break;
            }
            end_tok = jet_parser_peek_prev_tok(p);
            jet_ast_node_unop unop;
            unop.op_type = cur_tok->type;
            unop.expr_nid = rhs_nid;
             
            jet_ast_node node;
            JET_ASSERT(jet_ast_node_init(&node, AST_UNOP, 
                        start_tok->span.start, end_tok->span.end, 
                        start_tok->span.line, start_tok->span.col));
            node.as.unop = unop;
            out_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
            break;
        }
    }  

    if(out_nid == INVALID_NID)
    {
        return INVALID_NID;
    }

    //postfix expression call evaluation
    while(jet_parser_peekn_tok_type(p, 0) == TOK_LPAR)
    {
        jet_da arg_da;
        JET_ASSERT(jet_da_init(&arg_da, 2, sizeof(node_id)));
        jet_parser_consume_tok(p);        
        while(jet_parser_peekn_tok_type(p, 0) != TOK_RPAR)
        {
            node_id arg_nid = jet_parser_parse_expr(p, 0);
            if(arg_nid == INVALID_NID)
            {
                jet_da_dispose(&arg_da);
                return INVALID_NID;
            }
            JET_ASSERT(jet_da_append(&arg_da, (const void*)&arg_nid));
            if(jet_parser_peekn_tok_type(p, 0) == TOK_COMMA)
                jet_parser_consume_tok(p);
            else break;
        }
        
        end_tok = jet_parser_expect_tok(p, TOK_RPAR);
        if(end_tok == NULL)
        {
            return INVALID_NID;
        }

        if(jet_da_is_empty(&arg_da))
            jet_da_dispose(&arg_da);
        
        jet_ast_node_call call;
        if(out_nid == INVALID_NID)
        {
            return INVALID_NID;
        }

        call.callee_nid = out_nid;
        call.arg_nid_da = arg_da;

        jet_ast_node call_base;
        JET_ASSERT(jet_ast_node_init(&call_base, AST_CALL, 
                    start_tok->span.start, end_tok->span.end, 
                    start_tok->span.line, start_tok->span.col));

        call_base.as.call = call;
        out_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&call_base);
    }
    return out_nid;
}

static jet_result jet_parser_parse_primary_term(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    jet_result res;
    const jet_token* start_tok = jet_parser_peek_tok(p);
    switch(start_tok->type)
    {
        default:
            break;
        case TOK_IDENT:
        case TOK_KWD_INT:
        case TOK_KWD_BOOL:
        case TOK_KWD_FLOAT:
        case TOK_KWD_STR:
        case TOK_KWD_CHAR:
            res = jet_parser_lit_parse(p);
            break;
        case TOK_LPAR:
            jet_parser_consume_tok(p);
            res = jet_parser_parse_expr(p, 0);
            jet_parser_expect_tok(p, TOK_RPAR);
            break;
        case TOK_MINUS:
        case TOK_NOT:
            res = jet_parser_unop_parse(jet_parser* p);
            break;
    }
}

