#include <jet_parser.h>
#include <jet_parser_ops.h>
#include <jet_ast.h>
#include <jet_ast_node.h>
#include <jet_conv.h>
#include <jet_sb.h>
#include <jet_logger.h>

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

// TRAVERSING UTILS
static const jet_token* jet_parser_peek_prev_tok(jet_parser* p);
static const jet_token* jet_parser_peek_tok(jet_parser* p);
static const jet_token* jet_parser_peekn_tok(jet_parser* p, size_t n);
static const jet_token* jet_parser_consume_tok(jet_parser* p);
static const jet_token* jet_parser_expect_tok(jet_parser* p, jet_token_type tok_type);
static jet_token_type jet_parser_peekn_tok_type(jet_parser* p, size_t n);

// PARSING
static const char* jet_parser_create_type_name(const jet_token* tok, bool* is_primitive);
static bool jet_parser_is_type_tok(jet_token_type tok_type);
static bool jet_parser_is_vdecl(jet_parser* p);
static bool jet_parser_is_func_head(jet_parser* p);
static node_id jet_parser_parse_next_stmt(jet_parser* p);
static node_id jet_parser_parse_expr_stmt(jet_parser* p);
static node_id jet_parser_parse_expr(jet_parser* p, size_t min_prec);
static node_id jet_parser_parse_primary(jet_parser* ast);
static node_id jet_parser_prog_parse(jet_parser* p);
static node_id jet_parser_block_parse(jet_parser* p);
static node_id jet_parser_ident_parse(jet_parser* p);
static node_id jet_parser_tdecl_parse(jet_parser* p);
static node_id jet_parser_vdecl_parse(jet_parser* p);
static node_id jet_parser_lit_parse(jet_parser* p);
static node_id jet_parser_func_parse(jet_parser* p);
static node_id jet_parser_parse_fparam(jet_parser* p);

// EXPOSED===
bool jet_parser_init(jet_parser* p, const jet_da* tok_da, jet_ast* ast)
{
    if(!p || !tok_da || !ast)
    {
        JET_ERROR( "err: could not init parser, null param.\n");
        return false;
    }
    memset(p, 0 , sizeof(*p));
    p->tok_da = tok_da;
    p->ast = ast;
    p->tok_cursor = 0;
    return true;
}

bool jet_parser_dispose(jet_parser* p)
{
    if(!p) return false;
    memset(p, 0, sizeof(*p));
    return true;
}

bool jet_parser_parse(jet_parser* p)
{
    assert(p != NULL && "cannot parse, parser is null.");
    if(jet_da_count(p->tok_da) == 0)
        return true;

    jet_token_type t = TOK_EOF; 
    while(true)
    {
        t = jet_parser_peekn_tok_type(p, 0); 
        if(t == TOK_EOF) break;
        if(t == TOK_INV)
        {
            const jet_token* tk = jet_parser_peek_tok(p);
            JET_ERROR( "err: at [line:%"PRIu32", col:%"PRIu32"] cannot continue parsing, invalid token encountered.\n", tk->line, tk->col);
            return false;
        }
        node_id nid = jet_parser_parse_next_stmt(p);
        if(nid == INVALID_NID)
        {
            JET_ERROR( "error: cannot continue parsing, unable to parse next stmt.\n");        
            return false;
        }
        jet_ast_push_nid(p->ast, nid);
    }
    return true;
}

static const char* jet_parser_create_type_name(const jet_token* tok, bool* is_primitive)
{
    jet_token_type tok_type = tok->type;
    *is_primitive = true;
    
    jet_sb sb;
    if(!jet_sb_init(&sb, 32))
    {
        JET_ERROR( "err: cannot parse, failed to get type name, unable to init sb.\n");
        return NULL;
    }

    switch(tok_type)
    {
        default:
        {
            JET_ERROR( "err: token type (enum-id: %d) is not a type token.\n", (int)tok_type);
            goto fail;
        }
        case TOK_IDENT: 
        {
            *is_primitive = false;
            const char* tok_str = jet_token_strdup(tok);
            if(!tok_str)
            {
                JET_ERROR( "err: cannot parse, failed to dup token str.\n");
                goto fail;
            }
            jet_sb_append_cstr(&sb, tok_str);
            free((void*)tok_str);
            break;
        }
        case TOK_KWD_INT:
        {
            jet_sb_append_cstr(&sb, "int");
            break;
        }
        case TOK_KWD_FLOAT: 
        {
            jet_sb_append_cstr(&sb, "float");
            break;
        }
        case TOK_KWD_STR:
        {
            jet_sb_append_cstr(&sb, "str");
            break;
        }
        case TOK_KWD_BOOL: 
        {
            jet_sb_append_cstr(&sb, "bool");
            break;
        }
        case TOK_KWD_CHAR:
        {
            jet_sb_append_cstr(&sb, "char");
            break;
        }
    }

    const char* name = jet_sb_dup(&sb);
    if(!name)
    {
        JET_ERROR( "err: cannot parse, failed to get type name, unable to dup sb.\n");
        goto fail;
    }
    
    jet_sb_dispose(&sb);
    return name;

fail:
    jet_sb_dispose(&sb);
    return NULL;
}

// DEF=== 
static const jet_token* jet_parser_peek_prev_tok(jet_parser* p)
{
    if(p->tok_cursor == 0)
        return NULL;
    return (const jet_token*)jet_da_get(p->tok_da, p->tok_cursor - 1);
}

static const jet_token* jet_parser_peekn_tok(jet_parser* p, size_t n)
{ 
    assert(p != NULL && "cannot peek nth tok, parser is null");
    size_t count = jet_da_count(p->tok_da);
    if(count == 0 || n >= count - p->tok_cursor) 
        return NULL; 
    return (const jet_token*)jet_da_get(p->tok_da, p->tok_cursor + n);
}

static const jet_token* jet_parser_peek_tok(jet_parser* p)
{
    assert(p != NULL && "cannot peek tok, parser is null.");
    return jet_parser_peekn_tok(p, 0);
}

static jet_token_type jet_parser_peekn_tok_type(jet_parser* p, size_t n)
{
    assert(p != NULL && "cannot peek nth tok type, parser is null");
    const jet_token* tok = jet_parser_peekn_tok(p, n);
    if(tok == NULL)
        return TOK_EOF;
    return tok->type;
}

static const jet_token* jet_parser_expect_tok(jet_parser* p, jet_token_type tok_type)
{
    assert(p != NULL && "cannot expect tok, parser is null");
    const jet_token* tok = jet_parser_peek_tok(p);
    if(!tok) return NULL;
    if(tok->type != tok_type)
    {
        JET_ERROR( "error: expected token type (id: %d) but encountered (id: %d).\n", (int)tok_type, (int)tok->type);
        return NULL;
    }
    return jet_parser_consume_tok(p);
}

static const jet_token* jet_parser_consume_tok(jet_parser* p)
{
    assert(p != NULL && "cannot consume tok, parser is null");
    if(p->tok_cursor >= jet_da_count(p->tok_da))
        return NULL;
    p->tok_cursor++;
    return (const jet_token*)jet_da_get(p->tok_da , p->tok_cursor - 1);
}

// PARSING ==============================================================================

static bool jet_parser_is_type_tok(jet_token_type tok_type)
{
    switch(tok_type)
    {
        case TOK_KWD_INT:
        case TOK_KWD_FLOAT:
        case TOK_KWD_STR:
        case TOK_KWD_BOOL:
        case TOK_KWD_CHAR:
        case TOK_KWD_VOID:
            return true;
        default:
            return false;
    }
}

static bool jet_parser_is_vdecl(jet_parser* p)
{
    assert(p != NULL && "cannot parse next stmt, parser is null.");
    jet_token_type t0 = jet_parser_peekn_tok_type(p, 0);
    return jet_parser_is_type_tok(t0) && 
           jet_parser_peekn_tok_type(p, 1) == TOK_IDENT &&
           (jet_parser_peekn_tok_type(p, 2) == TOK_ASG || 
            jet_parser_peekn_tok_type(p, 2) == TOK_SEMI);
}

static bool jet_parser_is_func_head(jet_parser* p)
{
    assert(p != NULL && "cannot parse next stmt, parser is null.");
    jet_token_type t = jet_parser_peekn_tok_type(p, 0);
    return jet_parser_is_type_tok(t) &&
           jet_parser_peekn_tok_type(p, 1) == TOK_IDENT &&
           jet_parser_peekn_tok_type(p, 2) == TOK_LPAR;
}

static node_id jet_parser_parse_next_stmt(jet_parser* p)
{
    assert(p != NULL && "cannot parse next stmt, parser is null.");
    node_id parsed_nid = INVALID_NID;
    jet_token_type t = jet_parser_peekn_tok_type(p, 0);
    
    if(t == TOK_EOF)
        return INVALID_NID;
    else if(t == TOK_INV)
    {
        JET_ERROR( "error: cannot parse next stmt, invalid token.\n");
        return INVALID_NID; 
    }    
    else if(t == TOK_KWD_PROG)
    {
        parsed_nid = jet_parser_prog_parse(p);
        if(parsed_nid == INVALID_NID)
        {
            JET_ERROR( "error: unable to parse next stmt (prog)\n");
            return INVALID_NID;
        }
    }
    else if(jet_parser_is_vdecl(p))
    {
        parsed_nid = jet_parser_vdecl_parse(p);
        if(parsed_nid == INVALID_NID)
        {
            JET_ERROR( "error: cannot parse next stmt (vdecl)\n");
            return INVALID_NID;
        }
    }
    else if(jet_parser_is_func_head(p))
    { 
        parsed_nid = jet_parser_func_parse(p);
        if(parsed_nid == INVALID_NID)
        {
            JET_ERROR( "error: cannot parse next stmt (func)\n");
            return INVALID_NID;
        }
    }
    else 
    {
        parsed_nid = jet_parser_parse_expr_stmt(p);
        if(parsed_nid == INVALID_NID)
        {
            JET_ERROR( "cannot parse next stmt, expected expression statement.\n");
            return INVALID_NID;
        }
    }

    if(parsed_nid == INVALID_NID)
        JET_ERROR( "error: unable to parse next stmt, no valid stmt sequences parsed.\n");

    return parsed_nid;
}

static node_id jet_parser_parse_expr_stmt(jet_parser* p)
{
    assert(p != NULL && "cannot parse expr stmt, parser is null.");
    node_id expr_nid = jet_parser_parse_expr(p, 0);
    if(expr_nid == INVALID_NID)
    {
        JET_ERROR( "error: cannot parse expr stmt, expected expr node.\n");
        return INVALID_NID;
    }
    if(jet_parser_expect_tok(p, TOK_SEMI) == NULL)
    {
        JET_ERROR( "err: cannot parse expr_stmt, expected TOK_SEMI.\n");
        return INVALID_NID;
    }
    return expr_nid;
}

static node_id jet_parser_prog_parse(jet_parser* p)
{
    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_KWD_PROG);
    if(start_tok == NULL)
    {
        JET_ERROR( "err: cannot parse prog, expected TOK_KWD_PROG\n");
        return INVALID_NID;
    }

    jet_ast_node_prog prog;
    prog.block_nid = jet_parser_block_parse(p);
    const jet_token* end_tok = jet_parser_peek_prev_tok(p);

    if(prog.block_nid == INVALID_NID)
    {
        JET_ERROR( "error: unable to parse prog block.\n");
        return INVALID_NID;
    }
 
    jet_ast_node node;
    if(!jet_ast_node_init(&node, AST_PROG, start_tok->span.start, end_tok->span.end))
    {
        JET_ERROR( "fatal: cannot init prog node.\n");
        abort();
    }
    node.as.prog = prog;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_block_parse(jet_parser* p)
{
    assert(p != NULL && "cannot parse block, parser is null.");
    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_LBRC);
    if(start_tok == NULL)
    {
        JET_ERROR( "err: cannot parse block, expected TOK_LBRC.\n");
        return INVALID_NID;
    }
    jet_ast_node_block block;
    if(!jet_da_init(&block.stmt_nid_da, 4, sizeof(node_id)))
    {
        JET_ERROR( "fatal: cannot parse block, could not init block.stmt_nid_da.\n");
        abort();
    }

    node_id stmt_nid = INVALID_NID;
    jet_token_type t = TOK_EOF;
    while(true)
    {
        t = jet_parser_peekn_tok_type(p, 0);
        if(t == TOK_EOF || t == TOK_INV)
        {
            JET_ERROR( "err: cannot parse block, unexpected token.\n");
            goto fail;
        }
        else if(t == TOK_RBRC)
            break;

        stmt_nid = jet_parser_parse_next_stmt(p);
        if(stmt_nid == INVALID_NID)
        {
            JET_ERROR( "err: cannot parse block, unable to parse next stmt.\n");
            goto fail;
        }
        jet_da_append(&block.stmt_nid_da, (const void*)&stmt_nid);
    }
    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_RBRC);
    if(end_tok == NULL)
    {
        JET_ERROR( "err: cannot parse block, expected TOK_RBRC.\n");
        goto fail;
    }

    jet_ast_node node;
    if(!jet_ast_node_init(&node, AST_BLOCK, start_tok->span.start, end_tok->span.end))
    {
        JET_ERROR( "fatal: failed to init block node.\n");
        abort();
    }
    node.as.block = block;  
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);

fail:
    jet_da_dispose(&block.stmt_nid_da);
    return INVALID_NID;
}

static node_id jet_parser_ident_parse(jet_parser* p)
{
    assert(p != NULL && "cannot parse ident, parser is null."); 
    const jet_token* tok = jet_parser_peek_tok(p);
    if(!tok)
    {
        JET_ERROR( "error: cannot parse ident, no valid tokens to peek.\n");
        return INVALID_NID;
    }
    if(tok->type != TOK_IDENT)
    {
        JET_ERROR( "error: cannot parse ident, token type mismatch.\n");
        return INVALID_NID;
    }

    jet_ast_node_ident ident;
    ident.str = (const char*)jet_token_strdup(tok);
    if(!ident.str)
    {
        JET_ERROR( "error: cannot parse ident, unable to create token string dup.\n");
        return INVALID_NID;
    }
    
    jet_parser_consume_tok(p);
    
    jet_ast_node node;
    if(!jet_ast_node_init(&node, AST_IDENT, tok->span.start, tok->span.end))
    {
        JET_ERROR( "fatal: cannot parse ident, failed to init node.\n");
        abort();
    }
    node.as.ident = ident;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_lit_parse(jet_parser* p)
{
    assert(p != NULL && "cannot parse lit, parser is null.");
    const jet_token* tok = jet_parser_peek_tok(p);
    if(!tok)
    {
        JET_ERROR( "err: cannot parse lit, unable to peek tok.\n");
        return INVALID_NID;
    }
    jet_ast_node_lit lit;
    switch(tok->type)
    {
        default:
            JET_ERROR( "error: could not parse lit, no type representation for token: %d.\n", (int)tok->type);
            return INVALID_NID;
        case TOK_KWD_TRUE:
        {
            lit.tkind = JET_TYPE_BOOL;
            lit.as.b = true;
            break;
        }
        case TOK_KWD_FALSE:
        {
            lit.tkind = JET_TYPE_BOOL;
            lit.as.b = false;
            break;
        }
        case TOK_LIT_INT:
        {
           lit.tkind = JET_TYPE_INT;
           lit.as.i = jet_conv_stoi(tok->lexeme, tok->span.end - tok->span.start); 
           break;
        }
        case TOK_LIT_FLOAT:
        {
            lit.tkind = JET_TYPE_FLOAT;
            lit.as.f = jet_conv_stof(tok->lexeme, tok->span.end - tok->span.start);
            break;
        }
        case TOK_LIT_CHAR:
        {
            lit.tkind = JET_TYPE_CHAR;
            lit.as.c = *(tok->lexeme);
            break;
        }
        case TOK_LIT_STR:
        {
            lit.tkind = JET_TYPE_STR;
            lit.as.s = jet_token_strdup(tok); 
            if(!lit.as.s)
            {
                JET_ERROR( "err: cannot parse lit, failed to create tok strdup.\n");
                return INVALID_NID;
            }
            break;
        }
    }
    jet_parser_consume_tok(p);

    jet_ast_node node;
    if(!jet_ast_node_init(&node, AST_LIT, tok->span.start, tok->span.end))
    {
        JET_ERROR( "fatal: failed to parse lit, unable to init node.\n");
        abort();
    }
    node.as.lit = lit;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_tdecl_parse(jet_parser* p)
{
    assert(p != NULL && "cannot parse tdecl, parser is null.");
    const jet_token* tok = jet_parser_consume_tok(p);

    jet_ast_node_tdecl tdecl;
    tdecl.tname = jet_parser_create_type_name(tok, &tdecl.is_primitive);
    tdecl.byte_size = 4;
    jet_ast_node node;
    if(!jet_ast_node_init(&node, AST_TYPE_DECL, tok->span.start, tok->span.end))
    {
        JET_ERROR( "fatal: failed to parse tdecl, unable to init node.\n");
        abort();
    }
    node.as.tdecl = tdecl;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_vdecl_parse(jet_parser* p) 
{
    assert(p != NULL && "cannot parse vdecl, parser is null.");
    const jet_token* start_tok = jet_parser_peek_tok(p);

    jet_ast_node_vdecl vdecl;
    vdecl.tdecl_nid = jet_parser_tdecl_parse(p);
    if(vdecl.tdecl_nid == INVALID_NID)
    {
        JET_ERROR( "err: cannot parse vdecl, expected tdecl.\n");
        return INVALID_NID;
    }
    
    vdecl.ident_nid = jet_parser_ident_parse(p);
    if(vdecl.ident_nid == INVALID_NID)
    {
        JET_ERROR( "err: cannot parse vdecl, expected ident.\n");
        return INVALID_NID;
    }

    vdecl.init_value_nid = INVALID_NID;
    jet_token_type tok_type = jet_parser_peekn_tok_type(p, 0);
    if(tok_type == TOK_ASG)
    {
        jet_parser_consume_tok(p);    
        vdecl.init_value_nid = jet_parser_parse_expr(p, 0);
        if(vdecl.init_value_nid == INVALID_NID)
        {
            JET_ERROR( "error: cannot parse vdecl, expected expr value after asg operator.\n");
            return INVALID_NID;
        } 
    }
    
    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_SEMI);
    if(end_tok == NULL)
    {
        JET_ERROR( "err: cannot parse, expected TOK_SEMI.\n");
        return INVALID_NID;
    }
    
    jet_ast_node node;
    if(!jet_ast_node_init(&node, AST_VAR_DECL, start_tok->span.start, end_tok->span.end))
    {
        JET_ERROR( "fatal: failed to parse vdecl, unable to init node.\n");
        abort();
    } 
    node.as.vdecl = vdecl;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_func_parse(jet_parser* p) 
{
    assert(p != NULL && "cannot parse func, parser is null.");
    const jet_token* start_tok = jet_parser_peek_tok(p);

    jet_ast_node_fdecl fdecl;
    node_id ret_tdecl_nid = jet_parser_tdecl_parse(p); 
    fdecl.ident_nid = jet_parser_ident_parse(p);
    
    if( !jet_da_init(&fdecl.ret_tdecl_nid_da, 1, sizeof(node_id)) )
    {
        JET_ERROR( "fatal: cannot parse func, unable to init fdecl.ret_tdecl_nid_da.\n");
        abort();
    }

    if(!jet_da_append(&fdecl.ret_tdecl_nid_da, (const void*)&ret_tdecl_nid))
    {
        JET_ERROR( "err: cannot prase func, failed to append to ret_tdecl_nid_da.\n");
        jet_da_dispose(&fdecl.ret_tdecl_nid_da);
        return INVALID_NID;
    }

    if(jet_parser_expect_tok(p, TOK_LPAR) == NULL)
    {
        JET_ERROR( "err: cannot parse, expected TOK_LPAR.\n");
        jet_da_dispose(&fdecl.ret_tdecl_nid_da);
        return INVALID_NID;
    }

    if( !jet_da_init(&fdecl.param_nid_da, 2, sizeof(node_id)) )
    {
        JET_ERROR( "fatal: cannot parse func, unable to init fdecl.param_nid_da.\n");
        jet_da_dispose(&fdecl.ret_tdecl_nid_da);
        abort();
    }

    node_id vdecl_nid = INVALID_NID;
    jet_token_type t = TOK_EOF;
    while(jet_parser_peekn_tok_type(p, 0) != TOK_RPAR && jet_parser_peekn_tok_type(p, 0) != TOK_EOF)
    {
        vdecl_nid = jet_parser_parse_fparam(p);
        if(vdecl_nid == INVALID_NID)
        {
            JET_ERROR( "error: cannot parse func, unable to parse parameter.\n");
            goto fail;
        }

        if(!jet_da_append(&fdecl.param_nid_da, (const void*)&vdecl_nid))
        {
            JET_ERROR( "err: cannot parse func, failed to append to param_nid_da.\n");
            goto fail;
        }
        
        t = jet_parser_peekn_tok_type(p, 0);
        switch(t)
        {
            case TOK_RPAR:
                break;
            case TOK_COMMA:
                jet_parser_consume_tok(p);
                break;
            case TOK_EOF:
                JET_ERROR( "error: cannot parse func, EOF reached.\n");
                goto fail;
            case TOK_INV:
                JET_ERROR( "error: cannot parse func, invalid token encountered.\n");
                goto fail;
            default:
                JET_ERROR( "error: cannot parse func, unexpected token (type-enum-id: %d) encountered.\n", (int)t);
                goto fail;
        }
    }

    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_RPAR);
    if(end_tok == NULL)
    {
        JET_ERROR( "err: cannot parse func, expected TOK_RPAR.\n");
        goto fail;
    }

    jet_ast_node fdecl_base;
    if(!jet_ast_node_init(&fdecl_base, AST_FUNC_DECL, start_tok->span.start, end_tok->span.end))
    {
        JET_ERROR( "fatal: failed to parse func, unable to init node.\n");
        abort();
    }
    fdecl_base.as.fdecl = fdecl;

    node_id out_func_nid = INVALID_NID;
    if(jet_parser_peekn_tok_type(p, 0) == TOK_LBRC)
    {
        jet_ast_node_fdef fdef;
        fdef.block_nid = jet_parser_block_parse(p);
        if(fdef.block_nid == INVALID_NID)
        {
            JET_ERROR( "err: cannot parse func, unable to parse func definiton block.\n");
            goto fail;
        }
        fdef.fdecl_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&fdecl_base);
        end_tok = jet_parser_peek_prev_tok(p);
        jet_ast_node fdef_base;
        if(!jet_ast_node_init(&fdef_base, AST_FUNC_DEF, start_tok->span.start, end_tok->span.end))
        {
            JET_ERROR( "fatal: failed to parse func, unable to init node.\n");
            abort();
        }
        fdef_base.as.fdef = fdef;
        out_func_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&fdef_base); 
    }
    else
    {
        out_func_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&fdecl_base);
    }
    return out_func_nid;

fail:
    jet_da_dispose(&fdecl.ret_tdecl_nid_da);
    jet_da_dispose(&fdecl.param_nid_da);
    return INVALID_NID;
}

static node_id jet_parser_parse_fparam(jet_parser* p)
{
    assert(p != NULL && "cannot parse fparam, parser is null.");
    const jet_token* start_tok = jet_parser_peek_tok(p);
    jet_ast_node_vdecl vdecl;
    vdecl.tdecl_nid = jet_parser_tdecl_parse(p);
    if(vdecl.tdecl_nid == INVALID_NID)
    {
        JET_ERROR( "error: cannot parse vdecl, expected tdecl.\n");
        return INVALID_NID;
    }

    vdecl.ident_nid = jet_parser_ident_parse(p);
    if(vdecl.ident_nid == INVALID_NID)
    {
        JET_ERROR( "error: cannot parse vdecl, expected ident after tdecl.\n");
        return INVALID_NID;
    }

    vdecl.init_value_nid = INVALID_NID;
    jet_token_type tok_type = jet_parser_peekn_tok_type(p, 0);
    if(tok_type == TOK_ASG)
    {
        jet_parser_consume_tok(p);    
        vdecl.init_value_nid = jet_parser_parse_expr(p, 0);
        if(!vdecl.init_value_nid)
        {
            JET_ERROR( "error: cannot parse vdecl, expected expr value after asg operator.\n");
            return INVALID_NID;
        } 
    }
    const jet_token* end_tok = jet_parser_peek_prev_tok(p); 
    jet_ast_node node;
    if(!jet_ast_node_init(&node, AST_VAR_DECL, start_tok->span.start, end_tok->span.end))
    {
        JET_ERROR( "fatal: failed to parse fparam, unable to init node.\n");
        abort();
    }
    node.as.vdecl = vdecl;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_parse_expr(jet_parser* p, size_t min_prec)
{
    assert(p != NULL && "cannot parse expr, parser is null.");
    const jet_token* start_tok = jet_parser_peek_tok(p);
    node_id lhs_nid = jet_parser_parse_primary(p);
    if(lhs_nid == INVALID_NID)
    {
        JET_ERROR( "wrn: cannot parse expression, expected primary lhs.\n");
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
            JET_ERROR( "error: cannot parse expression, expected rhs expr after operator.\n");
            return INVALID_NID;
        }
        
        const jet_token* end_tok = jet_parser_peek_prev_tok(p);

        jet_ast_node_binop binop;
        binop.lhs_nid = lhs_nid;
        binop.rhs_nid = rhs_nid;
        binop.op_type = op_tok->type;
        
        jet_ast_node node;
        if(!jet_ast_node_init(&node, AST_BINOP, start_tok->span.start, end_tok->span.end))
        {
            JET_ERROR( "fatal: failed to parse binop, unable to init node.\n");
            abort();
        }
        node.as.binop = binop;
        lhs_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
    }
    return lhs_nid;
}

static node_id jet_parser_parse_primary(jet_parser* p)
{
    assert(p != NULL && "err: cannot parse primary expr, parser is null.");
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
            JET_ERROR( "err: expected primary expression.\n");
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
            out_nid = jet_parser_ident_parse(p);
            if(out_nid == INVALID_NID)
            {
                JET_ERROR( "err: cannot parse primary, unable to parse ident.\n");
                return INVALID_NID;
            }
            break;
        }
        case TOK_LPAR:
        {
            jet_parser_consume_tok(p);
            out_nid = jet_parser_parse_expr(p, 0);
            cur_tok = jet_parser_peek_tok(p);
            if(cur_tok == NULL || cur_tok->type != TOK_RPAR)
            {
                JET_ERROR( "err: expected ')' after primary expression.\n");
                return INVALID_NID;
            }
            jet_parser_consume_tok(p);
            break;
        }
        case TOK_NOT:
        case TOK_MINUS:
        {
            jet_parser_consume_tok(p);
            node_id rhs_nid = jet_parser_parse_expr(p, PREC_PREFIX);
            if(rhs_nid == INVALID_NID)
            {
                JET_ERROR( "err: expected expr after unary operator\n");
                return INVALID_NID;
            }
            end_tok = jet_parser_peek_prev_tok(p);
            jet_ast_node_unop unop;
            unop.op_type = cur_tok->type;
            unop.expr_nid = rhs_nid;
             
            jet_ast_node node;
            if(!jet_ast_node_init(&node, AST_UNOP, start_tok->span.start, end_tok->span.end))
            {
                JET_ERROR( "fatal: failed to parse primary unop, unable to init node.\n");
                abort();
            }
            node.as.unop = unop;
            out_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
            break;
        }
    }  

    if(out_nid == INVALID_NID)
    {
        JET_ERROR( "err: no valid primary expressions parsed.\n");
        return INVALID_NID;
    }

    //postfix expression call evaluation
    while(jet_parser_peekn_tok_type(p, 0) == TOK_LPAR)
    {
        jet_da arg_da;
        if( !jet_da_init(&arg_da, 2, sizeof(node_id)) )
        {
            JET_ERROR( "fatal: cannot parse primary expr, unable to init arg_da.\n");
            abort();
        }
        jet_parser_consume_tok(p);        
        while(jet_parser_peekn_tok_type(p, 0) != TOK_RPAR)
        {
            node_id arg_nid = jet_parser_parse_expr(p, 0);
            if(arg_nid == INVALID_NID)
            {
                JET_ERROR( "error: cannot parse primary expr, unable to parse call arg.\n");
                jet_da_dispose(&arg_da);
                return INVALID_NID;
            }
            if(!jet_da_append(&arg_da, (const void*)&arg_nid))
            {
                JET_ERROR( "err: cannot parse primary expr, unable append to arg_da.\n");
                jet_da_dispose(&arg_da);
                return INVALID_NID;
            }

            if(jet_parser_peekn_tok_type(p, 0) == TOK_COMMA)
                jet_parser_consume_tok(p);
            else break;
        }
        
        end_tok = jet_parser_expect_tok(p, TOK_RPAR);
        if(end_tok == NULL)
        {
            JET_ERROR( "err: cannot parse, expected TOK_RPAR.\n");
            jet_da_dispose(&arg_da);
            return INVALID_NID;
        }

        if(jet_da_is_empty(&arg_da))
            jet_da_dispose(&arg_da);
        
        jet_ast_node_call call;
        call.callee_nid = out_nid;
        call.arg_nid_da = arg_da;

        jet_ast_node call_base;
        if(!jet_ast_node_init(&call_base, AST_CALL, start_tok->span.start, end_tok->span.end))
        {
            JET_ERROR( "fatal: failed to parse primary expr fcall, unable to init node.\n");
            abort();
        }
        call_base.as.call = call;
        out_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&call_base);
    }
    return out_nid;
}

