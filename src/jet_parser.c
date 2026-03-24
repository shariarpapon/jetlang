#include <jet_parser.h>
#include <jet_parser_ops.h>
#include <jet_ast.h>
#include <jet_ast_node.h>
#include <jet_conv.h>
#include <jet_sb.h>
#include <jet_diag.h>

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
bool jet_parser_init(jet_parser* p, const char* filename, const jet_da* tok_da, jet_ast* ast)
{
    JET_ASSERT(p != NULL);
    if(!tok_da || !ast || !filename)
    {
        JET_LOG_ERROR("could not init parser, null param/s.");
        return false;
    }
    memset(p, 0 , sizeof(*p));
    p->filename = filename;
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
    JET_ASSERT(p != NULL);
    if(jet_da_count(p->tok_da) == 0)
        return true;

    jet_token_type t = TOK_EOF; 
    while(true)
    {
        t = jet_parser_peekn_tok_type(p, 0); 
        if(t == TOK_EOF) break;
        if(t == TOK_INV)
        {
            JET_DIAG_PARSING_HAULTED(jet_parser_peek_tok(p), "invalid token");
            return false;
        }
        node_id nid = jet_parser_parse_next_stmt(p);
        if(nid == INVALID_NID)
        {
            JET_DIAG_PARSING_HAULTED(jet_parser_peek_tok(p), "could not parse next stmt");
            return false;
        }
        jet_ast_push_nid(p->ast, nid);
    }
    return true;
}

static const char* jet_parser_create_type_name(const jet_token* tok, bool* is_primitive)
{
    JET_ASSERT(tok != NULL);
    JET_ASSERT(is_primitive != NULL);

    jet_token_type tok_type = tok->type;
    *is_primitive = true; 

    jet_sb sb;
    JET_ASSERT(jet_sb_init(&sb, 32));

    switch(tok_type)
    {
        default:
        {
            JET_DIAG_UNEXP_TOK_HERE(tok);
            goto fail;
        }
        case TOK_IDENT: 
        {
            *is_primitive = false;
            char* tok_str;
            jet_token_strdup(tok, &tok_str);
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
    JET_ASSERTM(name != NULL, "failed to dup sb-view");
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
    JET_ASSERT(p != NULL);
    size_t count = jet_da_count(p->tok_da);
    if(count == 0 || n >= count - p->tok_cursor) 
        return NULL; 
    return (const jet_token*)jet_da_get(p->tok_da, p->tok_cursor + n);
}

static const jet_token* jet_parser_peek_tok(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return jet_parser_peekn_tok(p, 0);
}

static jet_token_type jet_parser_peekn_tok_type(jet_parser* p, size_t n)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peekn_tok(p, n);
    if(tok == NULL)
        return TOK_EOF;
    return tok->type;
}

static const jet_token* jet_parser_expect_tok(jet_parser* p, jet_token_type tok_type)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peek_tok(p);
    if(!tok) return NULL;
    if(tok->type != tok_type)
    {
        JET_DIAG_UNEXP_TOK(tok, tok_type);
        return NULL;
    }
    return jet_parser_consume_tok(p);
}

static const jet_token* jet_parser_consume_tok(jet_parser* p)
{
    JET_ASSERT(p != NULL);
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
    JET_ASSERT(p != NULL);
    jet_token_type t0 = jet_parser_peekn_tok_type(p, 0);
    return jet_parser_is_type_tok(t0) && 
           jet_parser_peekn_tok_type(p, 1) == TOK_IDENT &&
           (jet_parser_peekn_tok_type(p, 2) == TOK_ASG || 
            jet_parser_peekn_tok_type(p, 2) == TOK_SEMI);
}

static bool jet_parser_is_func_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    jet_token_type t = jet_parser_peekn_tok_type(p, 0);
    return jet_parser_is_type_tok(t) &&
           jet_parser_peekn_tok_type(p, 1) == TOK_IDENT &&
           jet_parser_peekn_tok_type(p, 2) == TOK_LPAR;
}

static node_id jet_parser_parse_next_stmt(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    node_id parsed_nid = INVALID_NID;
    const jet_token* start_tok = jet_parser_peek_tok(p);
    jet_token_type t = jet_parser_peekn_tok_type(p, 0);
    
    if(t == TOK_EOF)
        return INVALID_NID;
    else if(t == TOK_INV)
    {
        JET_DIAG_INV_TOK(jet_parser_peek_tok(p));
        return INVALID_NID; 
    }    
    else if(t == TOK_KWD_PROG)
    {
        parsed_nid = jet_parser_prog_parse(p);
        if(parsed_nid == INVALID_NID)
        {
            JET_DIAG_CANT_PARSE_NODE(&start_tok->span, AST_PROG);
            return INVALID_NID;
        }
    }
    else if(jet_parser_is_vdecl(p))
    {
        parsed_nid = jet_parser_vdecl_parse(p);
        if(parsed_nid == INVALID_NID)
        {
            JET_DIAG_CANT_PARSE_NODE(&start_tok->span, AST_VAR_DECL);
            return INVALID_NID;
        }
    }
    else if(jet_parser_is_func_head(p))
    { 
        parsed_nid = jet_parser_func_parse(p);
        if(parsed_nid == INVALID_NID)
        {
            JET_DIAG_CANT_PARSE(&start_tok->span, "function");
            return INVALID_NID;
        }
    }
    else 
    {
        parsed_nid = jet_parser_parse_expr_stmt(p);
        if(parsed_nid == INVALID_NID)
        {
            JET_DIAG_EXPECTED_HERE(&start_tok->span, "an expression statement");
            return INVALID_NID;
        }
    }
    if(parsed_nid == INVALID_NID)
    {
        JET_DIAG_UNEXP_TOK_HERE(start_tok);
        JET_DIAG_CANT_PARSE(&start_tok->span, "next statement");
    }
    return parsed_nid;
}

static node_id jet_parser_parse_expr_stmt(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    node_id expr_nid = jet_parser_parse_expr(p, 0);
    if(expr_nid == INVALID_NID)
    {
        JET_DIAG_CANT_PARSE(&jet_parser_peek_tok(p)->span, "an expression node");
        return INVALID_NID;
    }
    if(jet_parser_expect_tok(p, TOK_SEMI) == NULL)
        return INVALID_NID;
    return expr_nid;
}

static node_id jet_parser_prog_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_KWD_PROG);
    if(start_tok == NULL)
        return INVALID_NID;

    jet_ast_node_prog prog;
    prog.block_nid = jet_parser_block_parse(p);
    const jet_token* end_tok = jet_parser_peek_prev_tok(p);

    if(prog.block_nid == INVALID_NID)
    {
        JET_DIAG_CANT_PARSE(&start_tok->span, AST_BLOCK);
        return INVALID_NID;
    }
 
    jet_ast_node node;
    JET_ASSERT(jet_ast_node_init(&node, 
                AST_PROG, 
                start_tok->span.start, 
                end_tok->span.end,
                start_tok->span.line,
                start_tok->span.col));

    node.as.prog = prog;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_block_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_expect_tok(p, TOK_LBRC);
    if(start_tok == NULL)
        return INVALID_NID;
    jet_ast_node_block block;
    JET_ASSERT(jet_da_init(&block.stmt_nid_da, 4, sizeof(node_id)));

    node_id stmt_nid = INVALID_NID;
    jet_token_type t = TOK_EOF;
    while(true)
    {
        t = jet_parser_peekn_tok_type(p, 0);
        if(t == TOK_EOF || t == TOK_INV)
        {
            JET_DIAG_UNEXP_TOK_HERE(jet_parser_peek_tok(p));
            goto fail;
        }
        else if(t == TOK_RBRC)
            break;

        stmt_nid = jet_parser_parse_next_stmt(p);
        if(stmt_nid == INVALID_NID)
            goto fail;

        jet_da_append(&block.stmt_nid_da, (const void*)&stmt_nid);
    }
    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_RBRC);
    if(end_tok == NULL)
        goto fail;

    jet_ast_node node;
    JET_ASSERT(jet_ast_node_init(&node, AST_BLOCK, 
                start_tok->span.start, 
                end_tok->span.end, 
                start_tok->span.line, 
                start_tok->span.col));
    node.as.block = block;  
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);

fail:
    jet_da_dispose(&block.stmt_nid_da);
    return INVALID_NID;
}

static node_id jet_parser_ident_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peek_tok(p);
    if(!tok)
    {
        JET_DIAG_CURSOR(p->tok_cursor, "failed to parse ident, no more tokens");
        return INVALID_NID;
    }
    if(tok->type != TOK_IDENT)
    {
        JET_DIAG_UNEXP_TOK(tok, TOK_IDENT);
        return INVALID_NID;
    }

    jet_ast_node_ident ident;
    jet_token_strdup(tok, &ident.str);
    JET_ASSERT(ident.str != NULL);

    jet_parser_consume_tok(p); 
    jet_ast_node node;
    JET_ASSERT(jet_ast_node_init(&node, AST_IDENT, tok->span.start, tok->span.end, tok->span.line, tok->span.col));
    node.as.ident = ident;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_lit_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peek_tok(p);
    if(!tok)
    {
        JET_DIAG_CURSOR(p->tok_cursor, "failed to parse lit, no more tokens");
        return INVALID_NID;
    }
    jet_ast_node_lit lit;
    switch(tok->type)
    {
        default:
            JET_DIAG_UNEXP_TOK_HERE(tok);
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
            jet_token_strdup(tok, &lit.as.s); 
            break;
        }
    }
    jet_parser_consume_tok(p);

    jet_ast_node node;
    JET_ASSERT(jet_ast_node_init(&node, AST_LIT, tok->span.start, tok->span.end, tok->span.line, tok->span.col));
    node.as.lit = lit;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_tdecl_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_consume_tok(p);
    jet_ast_node_tdecl tdecl;
    tdecl.tname = jet_parser_create_type_name(tok, &tdecl.is_primitive);
    tdecl.byte_size = 4;
    jet_ast_node node;
    JET_ASSERT(jet_ast_node_init(&node, AST_TYPE_DECL, tok->span.start, tok->span.end, tok->span.line, tok->span.col));
    node.as.tdecl = tdecl;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_vdecl_parse(jet_parser* p) 
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);
    jet_ast_node_vdecl vdecl;
    vdecl.tdecl_nid = jet_parser_tdecl_parse(p);
    if(vdecl.tdecl_nid == INVALID_NID)
    {
        JET_DIAG_CANT_PARSE_NODE(&jet_parser_peek_tok(p)->span, AST_TYPE_DECL);
        return INVALID_NID;
    }
    
    vdecl.ident_nid = jet_parser_ident_parse(p);
    if(vdecl.ident_nid == INVALID_NID)
    {
        JET_DIAG_CANT_PARSE_NODE(&jet_parser_peek_tok(p)->span, AST_IDENT);
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
            JET_DIAG_CANT_PARSE(&jet_parser_peek_tok(p)->span, "an expression node but one is expected here");
            return INVALID_NID;
        } 
    }
    
    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_SEMI);
    if(end_tok == NULL)
        return INVALID_NID;
    
    jet_ast_node node;
    JET_ASSERT(jet_ast_node_init(&node, AST_VAR_DECL, start_tok->span.start, end_tok->span.end, start_tok->span.line, start_tok->span.col));
    node.as.vdecl = vdecl;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_func_parse(jet_parser* p) 
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);

    jet_ast_node_fdecl fdecl;
    node_id ret_tdecl_nid = jet_parser_tdecl_parse(p); 
    fdecl.ident_nid = jet_parser_ident_parse(p);
    
    JET_ASSERT(!jet_da_init(&fdecl.ret_tdecl_nid_da, 1, sizeof(node_id)));
    JET_ASSERT(jet_da_append(&fdecl.ret_tdecl_nid_da, (const void*)&ret_tdecl_nid));

    if(jet_parser_expect_tok(p, TOK_LPAR) == NULL)
    {
        jet_da_dispose(&fdecl.ret_tdecl_nid_da);
        return INVALID_NID;
    }

    JET_ASSERT(jet_da_init(&fdecl.param_nid_da, 2, sizeof(node_id)));

    node_id vdecl_nid = INVALID_NID;
    jet_token_type t = TOK_EOF;
    while(jet_parser_peekn_tok_type(p, 0) != TOK_RPAR && jet_parser_peekn_tok_type(p, 0) != TOK_EOF)
    {
        vdecl_nid = jet_parser_parse_fparam(p);
        if(vdecl_nid == INVALID_NID)
        {
            JET_DIAG_CANT_PARSE(&jet_parser_peek_tok(p)->span, "function parameter");
            goto fail;
        }

        JET_ASSERT(jet_da_append(&fdecl.param_nid_da, (const void*)&vdecl_nid));
        
        t = jet_parser_peekn_tok_type(p, 0);
        switch(t)
        {
            case TOK_RPAR:
                break;
            case TOK_COMMA:
                jet_parser_consume_tok(p);
                break;
            case TOK_EOF:
                JET_DIAG_UNEXP_TOK_HERE(jet_parser_peek_tok(p));
                goto fail;
            case TOK_INV:
                JET_DIAG_INV_TOK(jet_parser_peek_tok(p));
                goto fail;
            default:
                JET_DIAG_UNEXP_TOK_HERE(jet_parser_peek_tok(p));
                goto fail;
        }
    }

    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_RPAR);
    if(end_tok == NULL)
        goto fail;

    jet_ast_node fdecl_base;
    JET_ASSERT(jet_ast_node_init(&fdecl_base, AST_FUNC_DECL, start_tok->span.start, end_tok->span.end, start_tok->span.line, start_tok->span.col));

    fdecl_base.as.fdecl = fdecl;
    node_id out_func_nid = INVALID_NID;
    if(jet_parser_peekn_tok_type(p, 0) == TOK_LBRC)
    {
        jet_ast_node_fdef fdef;
        fdef.block_nid = jet_parser_block_parse(p);
        if(fdef.block_nid == INVALID_NID)
            goto fail;
        fdef.fdecl_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&fdecl_base);
        end_tok = jet_parser_peek_prev_tok(p);

        jet_ast_node fdef_base;
        JET_ASSERT(jet_ast_node_init(&fdef_base, AST_FUNC_DEF, start_tok->span.start, end_tok->span.end, start_tok->span.line, start_tok->span.col));
        
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
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);
    jet_ast_node_vdecl vdecl;
    vdecl.tdecl_nid = jet_parser_tdecl_parse(p);
    if(vdecl.tdecl_nid == INVALID_NID)
    {
        JET_DIAG_CANT_PARSE_NODE(&jet_parser_peek_tok(p)->span, AST_TYPE_DECL);
        return INVALID_NID;
    }

    vdecl.ident_nid = jet_parser_ident_parse(p);
    if(vdecl.ident_nid == INVALID_NID)
    {
        JET_DIAG_CANT_PARSE_NODE(&jet_parser_peek_tok(p)->span, AST_IDENT);
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
            JET_DIAG_CANT_PARSE(&jet_parser_peek_tok(p)->span, "an expression node but one is expected here");
            return INVALID_NID;
        } 
    }
    const jet_token* end_tok = jet_parser_peek_prev_tok(p);  
    jet_ast_node node;
    JET_ASSERT(jet_ast_node_init(&node, AST_VAR_DECL, start_tok->span.start, end_tok->span.end, start_tok->span.line, start_tok->span.col));
    node.as.vdecl = vdecl;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static node_id jet_parser_parse_expr(jet_parser* p, size_t min_prec)
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);
    node_id lhs_nid = jet_parser_parse_primary(p);
    if(lhs_nid == INVALID_NID)
    {
        JET_DIAG_CANT_PARSE(&jet_parser_peek_tok(p)->span, "lhs expression node");
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
            JET_DIAG_CANT_PARSE(&jet_parser_peek_tok(p)->span, "rhs expression node");
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

static node_id jet_parser_parse_primary(jet_parser* p)
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
            JET_DIAG_UNEXP_TOK_HERE(cur_tok);
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
                JET_DIAG_CANT_PARSE_NODE(&cur_tok->span, AST_IDENT);
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
                JET_DIAG_CURSOR(p->tok_cursor, "expected ')' after primary expression");
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
                JET_DIAG_CANT_PARSE(&jet_parser_peek_tok(p)->span, "rhs expression with prefix precedence");
                return INVALID_NID;
            }
            end_tok = jet_parser_peek_prev_tok(p);
            jet_ast_node_unop unop;
            unop.op_type = cur_tok->type;
            unop.expr_nid = rhs_nid;
             
            jet_ast_node node;
            JET_ASSERT(jet_ast_node_init(&node, AST_UNOP, start_tok->span.start, end_tok->span.end, start_tok->span.line, start_tok->span.col));
            node.as.unop = unop;
            out_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
            break;
        }
    }  

    if(out_nid == INVALID_NID)
    {
        JET_DIAG_CANT_PARSE(&jet_parser_peek_tok(p)->span, "valid primary expression");
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
                JET_DIAG_CANT_PARSE(&jet_parser_peek_tok(p)->span, "call argument node but is expected");
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
            jet_da_dispose(&arg_da);
            return INVALID_NID;
        }

        if(jet_da_is_empty(&arg_da))
            jet_da_dispose(&arg_da);
        
        jet_ast_node_call call;
        call.callee_nid = out_nid;
        call.arg_nid_da = arg_da;

        jet_ast_node call_base;
        JET_ASSERT(jet_ast_node_init(&call_base, AST_CALL, start_tok->span.start, end_tok->span.end, start_tok->span.line, start_tok->span.col));
        call_base.as.call = call;
        out_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&call_base);
    }
    return out_nid;
}

