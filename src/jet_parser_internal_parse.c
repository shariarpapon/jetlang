#include <inttypes.h>
#include <string.h>

#include <jet_parser_internals.h>
#include <jet_conv.h>
#include <jet_sb.h>
#include <jet_logger.h>

jet_result jet_parser_parse_toplevel(jet_parser* p)
{
    JET_ASSERT(p != NULL);    
 
    const jet_token* start_tok = jet_parser_peek_tok(p);
    jet_token_type t = start_tok->type;
     
    if(t == TOK_EOF)
    {
        jet_error err = jet_err(JET_ERR_UNEXPECTED_EOF, start_tok->span);
        return jet_result_error(&err);
    }
    else if(t == TOK_INV)
    {
        jet_error err = jet_err(JET_ERR_INVALID_TOKEN, start_tok->span);
        return jet_result_error(&err);
    }
    else if(jet_parser_is_tdef(p))
        return jet_parser_tdef_parse(p);
    else if(jet_parser_is_prog_entry(p))
        return jet_parser_prog_parse(p);
    else if(jet_parser_is_vdecl(p))
        return jet_parser_vdecl_parse(p); 
    else if(jet_parser_is_func_head(p))
        return jet_parser_func_parse(p);
    else 
        return jet_parser_parse_expr_stmt(p);
}

jet_result jet_parser_parse_block_stmt(jet_parser* p)
{
    JET_ASSERT(p != NULL);    
 
    const jet_token* start_tok = jet_parser_peek_tok(p);
    jet_token_type t = start_tok->type;
     
    if(t == TOK_EOF)
    {
        jet_error err = jet_err(JET_ERR_UNEXPECTED_EOF, start_tok->span);
        return jet_result_error(&err);
    }
    else if(t == TOK_INV)
    {
        jet_error err = jet_err(JET_ERR_INVALID_TOKEN, start_tok->span);
        return jet_result_error(&err);
    }
    else if(jet_parser_is_vdecl(p))
        return jet_parser_vdecl_parse(p); 
    else 
        return jet_parser_parse_expr_stmt(p);
}


jet_result jet_parser_parse_expr_stmt(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    jet_result result = jet_parser_parse_expr(p, 0);
    if(result.success)
        jet_parser_expect_tok(p, TOK_SEMI);
    return result;
}

jet_result jet_parser_prog_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_KWD_PROG); 
    if(start_tok == NULL)
        return jet_parser_result_parsing_failed(AST_PROG, jet_parser_peek_tok(p));

    jet_result result = jet_parser_block_parse(p);
    if(!result.success)
        return result;
    const jet_token* end_tok = jet_parser_peek_prev_tok(p);
    jet_span span = jet_parser_token_range_span(start_tok, end_tok);
    jet_ast_node node = jet_astn_prog(span, result.as.nid); 
    return jet_parser_register_node(p, (const jet_ast_node*)&node);
}

jet_result jet_parser_block_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    bool da_init = false;
    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_LBRC);
    if(start_tok == NULL)
        goto fail;

    jet_da stmt_nid_da;
    da_init = jet_da_init(&stmt_nid_da, 4, sizeof(node_id));
    JET_ASSERT(da_init);

    jet_result result;
    jet_token_type t = TOK_EOF;
    const jet_token* token;
    bool unexp_eof = false;
    while(true)
    {
        token = jet_parser_peek_tok(p);
        t = token->type;
        if(t == TOK_EOF)
        {
            jet_error err = jet_err(JET_ERR_UNEXPECTED_EOF, token->span);
            jet_parser_push_err(p, (const jet_error*)&err);
            unexp_eof = true;
            break;
        }
        else if(t == TOK_INV)
        {
            jet_error err = jet_err(JET_ERR_INVALID_TOKEN, token->span);
            jet_parser_push_err(p, (const jet_error*)&err); 
            jet_parser_consume_tok(p);
            continue;
        }
        else if(t == TOK_RBRC)
        {
            break;
        }

        result = jet_parser_parse_block_stmt(p);
        if(result.success)
            jet_da_append(&stmt_nid_da, (const void*)&result.as.nid);
        else 
        {
            jet_parser_push_err(p, &result.as.error); 
            if(!jet_parser_sync_block(p))
                goto fail;
        }
    }

    const jet_token* end_tok;

    if(unexp_eof) 
        end_tok = jet_parser_peek_tok(p);
    else
        end_tok = jet_parser_expect_tok(p, TOK_RBRC); 

    jet_span span = jet_parser_token_range_span(start_tok, end_tok);
    jet_ast_node node = jet_astn_block(span, (const jet_da*)&stmt_nid_da);
    jet_da_dispose(&stmt_nid_da);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);

fail:
    if(da_init)
        jet_da_dispose(&stmt_nid_da);
    return jet_parser_result_parsing_failed(AST_BLOCK, jet_parser_peek_tok(p));
}

jet_result jet_parser_lit_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peek_tok(p);
    jet_type_kind tkind;
    const void* value;
    switch(tok->type)
    {
        default:
            return jet_parser_result_parsing_failed(AST_LIT, tok);
        case TOK_KWD_TRUE:
        case TOK_KWD_FALSE:
        {
            tkind = JET_TYPE_BOOL;
            bool* b = malloc(sizeof(bool));
            *b = tok->type == TOK_KWD_TRUE;
            value = (const void*)b;
            break;
        }
        case TOK_LIT_INT:
        {
           tkind = JET_TYPE_INT;
           int64_t* i = malloc(sizeof(int64_t));
           *i = (int64_t)jet_conv_stoi(tok->lexeme, tok->span.end - tok->span.start); 
           value = (const void*)i;
           break;
        }
        case TOK_LIT_FLOAT:
        {
            tkind = JET_TYPE_FLOAT;
            float* f = malloc(sizeof(float));
            *f = jet_conv_stof(tok->lexeme, tok->span.end - tok->span.start);
            value = (const void*)f;
            break;
        }
        case TOK_LIT_CHAR:
        {
            tkind = JET_TYPE_CHAR;
            char* c = malloc(sizeof(char));
            *c = *(tok->lexeme);
            value = (const void*)c;
            break;
        }
        case TOK_LIT_STR:
        {
            tkind = JET_TYPE_STR;
            char** s = malloc(sizeof(char*));
            jet_token_strdup(tok, s); 
            value = s;
            break;
        }
    }
    jet_parser_consume_tok(p);
    jet_ast_node node = jet_astn_lit(tkind, value);
    free(value);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);
}

jet_result jet_parser_tdef_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);

    if(!jet_parser_is_tdef(p))
        return jet_parser_result_parsing_failed(AST_TYPE_DEF, jet_parser_peek_tok(p));

    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_KWD_TYPE);
    JET_ASSERT(start_tok != NULL);

    const jet_token* ident_tok = jet_parser_expect_tok(p, TOK_IDENT);
    JET_ASSERT(ident_tok != NULL);

    jet_result block_result = jet_parser_block_parse(p);
    if(!block_result.success)
        return block_result;

    const jet_token* end_tok = jet_parser_peek_prev_tok(p);

    const jet_ident type_name = jet_parser_ident_from_tok(ident_tok);
    jet_parser_register_type_name(p, type_name);

    jet_span span = jet_parser_token_range_span(start_tok, end_tok);    
    jet_ast_node node = jet_astn_tdef(span, type_name, block_result.as.nid);

    return jet_parser_register_node(p, (const jet_ast_node*)&node);
}

jet_result jet_parser_tdecl_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL); 
    if(!jet_parser_is_tdecl(p))
        goto fail;
    
    const jet_token* tok = jet_parser_consume_tok(p);
    jet_type_kind tkind = jet_parser_type_kind_from_token(tok);
    if(tkind == JET_TYPE_INVALID)
        goto fail;

    const jet_ident type_name = jet_parser_ident_from_tok(tok);
    jet_ast_node node = jet_astn_tdecl(tok->span, type_name, tkind);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);

fail:
    return jet_parser_result_parsing_failed(AST_TYPE_DECL, jet_parser_peek_tok(p));
}

jet_result jet_parser_vdecl_parse(jet_parser* p) 
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);

    if(jet_parser_is_vdecl(p) == false)
        return jet_parser_result_parsing_failed(AST_VAR_DECL, start_tok);

    jet_result tdecl_res  = jet_parser_tdecl_parse(p);
    if(!tdecl_res.success)
        return tdecl_res;

    const jet_token* ref_name_tok = jet_parser_expect_tok(p, TOK_IDENT);
    const jet_ident ref_name = jet_parser_ident_from_tok(ref_name_tok);

    jet_token_type tok_type = jet_parser_peekn_tok_type(p, 0);
    jet_result init_value_res = {.success = false};
    if(tok_type == TOK_ASG)
    {
        const jet_token* asg_tok = jet_parser_consume_tok(p);    
        init_value_res = jet_parser_parse_expr(p, 0);
        if(!init_value_res.success)
        {
            const jet_token* fail_tok = jet_parser_peek_prev_tok(p);
            jet_span err_span = jet_parser_token_range(asg_tok, fail_tok);
            jet_error err = jet_err(JET_ERR_EXPECTED_EXPR, err_span);
            return jet_result_error(&err);
        }
    }
    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_SEMI);
    if(end_tok == NULL)
        end_tok = jet_parser_peek_prev_tok(p);

    jet_span node_span = jet_parser_token_range_span(start_tok, end_tok); 
    node_id init_value_nid = init_value_res.success ? init_value_res.as.nid : INVALID_NID;
    jet_ast_node node = jet_astn_vdecl(node_span, ref_name, tdecl_res.as.nid, init_value_nid); 
    return jet_parser_register_node(p, (const jet_ast_node*)&node);
}

jet_result jet_parser_func_parse(jet_parser* p) 
{
    JET_ASSERT(p != NULL);

    if(!jet_parser_is_func_head(p))
        goto fail;

    const jet_token* start_tok = jet_parser_peek_tok(p);
    
    jet_result ret_tdecl_res = jet_parser_tdecl_parse(p);
    if(!ret_tdecl_res.success)
        return ret_tdecl_res;

    const jet_token* ref_name_tok = jet_parser_expect_tok(p, TOK_IDENT);
    jet_ident ref_name = jet_parser_ident_from_tok(ref_name_tok);

    jet_parser_expect_tok(p, TOK_LPAR); 

    jet_da param_nid_da; 
    bool param_nid_da_init = jet_da_init(&param_nid_da, 2, sizeof(node_id));
    JET_ASSERT(param_nid_da_init == true);

    jet_result param_res = {.success = false};
    while(jet_parser_peekn_tok_type(p, 0) != TOK_RPAR && 
          jet_parser_peekn_tok_type(p, 0) != TOK_EOF &&
          jet_parser_peekn_tok_type(p, 0) != TOK_SEMI)
    {
        param_res = jet_parser_parse_fparam(p);
        if(!param_res.success)
        {
            if(!jet_parser_sync_fparam(p))
                goto fail; 
            continue;
        }
        
        JET_ASSERT(jet_da_append(&param_nid_da, (const void*)&param_res.as.nid));
    
        const jet_token* cur_tok = jet_parser_peek_tok(p);
        if(cur_tok->type == TOK_COMMA)
        {
            jet_parser_consume_tok(p);
            continue;
        }
    }

    const jet_token* fparam_end_tok = jet_parser_peek_tok(p);
    bool is_decl_only = false;
    switch(fparam_end_tok->type)
    {
        case TOK_SEMI:
        case TOK_EOF:
            is_decl_only = true;
            jet_parser_consume_tok(p);
            break;
        case TOK_RPAR:
            jet_parser_consume_tok(p);
            break;
        default: 
            goto fail;
    }

    jet_span fdecl_span = jet_parser_token_range_span(start_tok, fparam_end_tok);
    jet_ast_node fdecl_node = jet_astn_fdecl(fdecl_span, ref_name, ret_tdecl_res.as.nid, &param_nid_da);
    
    jet_da_dispose(&param_nid_da);
    param_nid_da_init = false;

    jet_result fdecl_res = jet_parser_register_node(p, &fdecl_node);

    if(is_decl_only)
        return fdecl_res;

    jet_result block_res = jet_parser_block_parse(p);
    if(!block_res.success)
    {
        jet_parser_push_err(p, &block_res.as.error);
        return fdecl_res;
    }

    jet_span fdef_span = jet_parser_token_range_span(start_tok, jet_parser_peek_prev_tok(p));
    jet_ast_node fdef_node = jet_astn_fdef(fdef_span, fdecl_res.as.nid, block_res.as.nid);
    return jet_parser_register_node(p, &fdef_node);

fail:
    if(param_nid_da_init) 
        jet_da_dispose(&param_nid_da);
    return jet_parser_result_parsing_failed(AST_FUNC_DECL, jet_parser_peek_tok(p));
}

jet_result jet_parser_parse_fparam(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);

    if(jet_parser_is_vdecl(p) == false)
        return jet_parser_result_parsing_failed(AST_VAR_DECL, start_tok);

    jet_result tdecl_res  = jet_parser_tdecl_parse(p);
    if(!tdecl_res.success)
        return tdecl_res;

    const jet_token* ref_name_tok = jet_parser_expect_tok(p, TOK_IDENT);
    const jet_ident ref_name = jet_parser_ident_from_tok(ref_name_tok);

    jet_token_type tok_type = jet_parser_peekn_tok_type(p, 0);
    jet_result init_value_res = {.success = false};
    if(tok_type == TOK_ASG)
    {
        const jet_token* asg_tok = jet_parser_consume_tok(p);    
        init_value_res = jet_parser_parse_expr(p, 0);
        if(!init_value_res.success)
        {
            const jet_token* fail_tok = jet_parser_peek_prev_tok(p);
            jet_span err_span = jet_parser_token_range(asg_tok, fail_tok);
            jet_error err = jet_err(JET_ERR_EXPECTED_EXPR, err_span);
            return jet_result_error(&err);
        }
    }
    jet_span node_span = jet_parser_token_range_span(start_tok, jet_parser_peek_prev_tok(p)); 
    node_id init_value_nid = init_value_res.success ? init_value_res.as.nid : INVALID_NID;
    jet_ast_node node = jet_astn_vdecl(node_span, ref_name, tdecl_res.as.nid, init_value_nid); 
    return jet_parser_register_node(p, &node);
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

jet_result jet_parser_parse_primary(jet_parser* p)
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
        {
            out_nid = jet_parser_lit_parse(p);
            break;
        }
        case TOK_IDENT:
        {
            out_nid = jet_parser_ident_from_tok(p);
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









