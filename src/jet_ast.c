/* 
 * NAMING CONVENTIONS
 *
 * peekn : peek_nth
 * astn  : ast_node_id 
 * tok   : token
 * nid   : node_id (size_t), 0 = invalid / NULL
 * da    : dynamic_array (from utils)
 * */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <jet_ast.h>
#include <jet_ast_node.h>
#include <jet_ast_op_prec.h>
#include <jet_conv.h>

struct jet_ast 
{
    jet_da tok_da;
    jet_da node_registry;
    jet_da top_nid_da;

    node_id prog_nid;
    size_t node_count;
    size_t tok_cursor;
};

static node_id jet_ast_register_node(jet_ast* ast, const jet_ast_node* node);
static const jet_ast_node* jet_ast_node_get(const jet_ast* ast, node_id nid);

// TOKEN TRAVERSING/UTILITY
static void jet_ast_push_nid(jet_ast* ast, node_id nid);
static const jet_token* jet_ast_peek_tok(jet_ast* ast);
static const jet_token* jet_ast_peekn_tok(jet_ast* ast, size_t n);
static const jet_token* jet_ast_consume_tok(jet_ast* ast);
static const jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type);
static jet_token_type jet_ast_peekn_tok_type(jet_ast* ast, size_t n);
static const char* jet_ast_get_type_name(jet_token_type tok_type);

// PARSING
static bool jet_ast_is_type_tok(jet_token_type tok_type);
static bool jet_ast_is_vdecl(jet_ast* ast);
static bool jet_ast_is_func_head(jet_ast* ast);

static node_id jet_astn_parse_next_stmt(jet_ast* ast);
static node_id jet_astn_parse_expr_stmt(jet_ast* ast);
static node_id jet_astn_parse_expr(jet_ast* ast, size_t min_prec);
static node_id jet_astn_parse_primary(jet_ast* ast);
static node_id jet_astn_prog_parse(jet_ast* ast);
static node_id jet_astn_block_parse(jet_ast* ast);
static node_id jet_astn_ident_parse(jet_ast* ast);
static node_id jet_astn_tdecl_parse(jet_ast* ast);
static node_id jet_astn_vdecl_parse(jet_ast* ast);
static node_id jet_astn_lit_parse(jet_ast* ast);

static node_id jet_astn_func_parse(jet_ast* ast);
static node_id jet_astn_parse_fparam(jet_ast* ast);

//ast should be zero initialized before init is called or risks mem leak
bool jet_ast_init(jet_ast* ast, const jet_da* tok_da)
{
    assert(ast != NULL && tok_da != NULL && "cannot init, ast or tok_da is null");
    
    //clone lexers token DA, allowing for safe disposal of lexer
    if(!jet_da_clone(&ast->tok_da, tok_da))
    {
        fprintf(stderr, "err: failed to init ast, unable to clone tok_da DA.\n");
        return false;
    }
    
    ast->prog_nid = INVALID_NID;
    ast->node_count = 0;
    ast->tok_cursor = 0;
    
    if( !jet_da_init(&ast->node_registry, 32, sizeof(jet_ast_node)) )
    {
        fprintf(stderr, "err: failed to init node_registry DA.\n");
        jet_da_dispose(&ast->tok_da);
        return false;
    }
    
    if( !jet_da_init(&ast->top_nid_da, 16, sizeof(node_id)) )
    {
        fprintf(stderr, "err: failed to init top_node_nid_da DA.\n");
        jet_da_dispose(&ast->tok_da);
        jet_da_dispose(&ast->node_registry);
        return false;
    }

    return true;
}

void jet_ast_dispose(jet_ast* ast)
{
    if(!ast) return;
    jet_da_dispose(&ast->tok_da);
    jet_da_dispose(&ast->node_registry);
    jet_da_dispose(&ast->top_nid_da);
    memset(ast, 0, sizeof(*ast));
}

//persists ast->tok_da
void jet_ast_reset(jet_ast* ast)
{
    assert(ast != NULL && "cannot reset, ast is null");
    jet_da_clear(&ast->node_registry);
    jet_da_clear(&ast->top_nid_da);
    ast->prog_nid = INVALID_NID;
    ast->node_count = 0;
    ast->tok_cursor = 0;
}

bool jet_ast_generate_nodes(jet_ast* ast)
{
    if(ast == NULL)
    {
        fprintf(stderr, "error: cannot generate ast nodes, ast is null.\n");
        return false;
    }

    if(jet_da_count(&ast->tok_da) == 0)
        return true;

    jet_ast_reset(ast);
    jet_token_type t = TOK_EOF;
    
    while(true)
    {
        t = jet_ast_peekn_tok_type(ast, 0);
        if(t == TOK_EOF) break;
        if(t == TOK_INV)
        {
            fprintf(stderr, "error: cannot generate ast nodes, invalid token encountered.\n");
            return false;
        }
        node_id nid = jet_astn_parse_next_stmt(ast);
        if(nid == INVALID_NID)
        {
            fprintf(stderr, "error: cannot generate ast, unable to parse next stmt.\n");        
            return false;
        }
        jet_ast_push_nid(ast, nid);
    }
    return true;
}

const jet_da* jet_ast_get_top_nid_da(const jet_ast* ast)
{
    assert(ast != NULL && "cannot get top_nid_da, ast is null");
    return (const jet_da*)&ast->top_nid_da;
}

node_id jet_ast_get_prog_nid(const jet_ast* ast)
{
    assert(ast != NULL && "cannot get prog nid, ast is null");
    return ast->prog_nid;
}

static const jet_ast_node* jet_ast_node_get(const jet_ast* ast, node_id nid)
{
    assert(ast != NULL && "cannot get node with specified nid, ast is null");
    if(nid == INVALID_NID)
    {
        fprintf(stderr, "err: cannot get node with id=%zu, this id internally represents invalid node.\n", (size_t)INVALID_NID);
        return NULL;
    }

    // force nid - 1 >= 0, because 0 represents invalid.
    const jet_ast_node* node = (const jet_ast_node*)jet_da_get(&ast->node_registry, nid - 1);
    if(!node)
    {
        fprintf(stderr, "err: node with id=%zu does not exist in registry.\n", nid);
        return NULL;
    }
    return node;
}

static node_id jet_ast_register_node(jet_ast* ast, const jet_ast_node* node)
{
    assert(ast != NULL && "cannot register node, ast is null");
    assert(node != NULL && "cannot register node, param node is null");
    jet_da_append(&ast->node_registry, (const void*)node);
    ast->node_count++;
    return ast->node_count;
}

static void jet_ast_push_nid(jet_ast* ast, node_id nid)
{
    const jet_ast_node* node = jet_ast_node_get(ast, nid);
    if(node == NULL)
    {
        fprintf(stderr, "err: nid=%zu does not exist in registry.\n", nid);
        return;
    }
    if(node->node_type == AST_PROG)
    {
        if(ast->prog_nid == INVALID_NID)
            ast->prog_nid = nid;
        else
        {
            fprintf(stderr, "error: cannot push node to ast, multiple program entry points.\n");
            return;
        }
    }
    else jet_da_append(&ast->top_nid_da, (const void*)&nid);
}

static const jet_token* jet_ast_peek_tok(jet_ast* ast)
{
    return jet_ast_peekn_tok(ast, 0);
}

static const jet_token* jet_ast_peekn_tok(jet_ast* ast, size_t n)
{ 
    assert(ast != NULL && "cannot peek tok, ast is null");
    size_t count = jet_da_count(&ast->tok_da);
    if(count == 0)
    {
        fprintf(stderr, "wrn: ast->tok_da is empty.\n");        
        return NULL; 
    }
    else if(n >= count - ast->tok_cursor)
    {
        fprintf(stderr, "wrn: cannot peak ahead %zu tokens (cursor=%zu), index out of bounds.\n", n, ast->tok_cursor);
        return NULL;
    }
    return (const jet_token*)jet_da_get(&ast->tok_da, ast->tok_cursor + n);
}

static const jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type)
{
    assert(ast != NULL && "cannot expect tok, ast is null");
    const jet_token* tok = jet_ast_peek_tok(ast);
    if(!tok) return NULL;
    if(tok->type != tok_type)
    {
        fprintf(stderr, "error: expected token type (id: %d) but encountered (id: %d).\n", (int)tok_type, (int)tok->type);
        return NULL;
    }
    return jet_ast_consume_tok(ast);
}

static const jet_token* jet_ast_consume_tok(jet_ast* ast)
{
    assert(ast != NULL && "cannot consume tok, ast is null");
    if(ast->tok_cursor >= jet_da_count(&ast->tok_da))
    {
        fprintf(stderr, "wrn: cannot consume token, cursor already at end.\n");
        return NULL;
    }
    ast->tok_cursor++;
    return (const jet_token*)jet_da_get(&ast->tok_da , ast->tok_cursor - 1);
}

static jet_token_type jet_ast_peekn_tok_type(jet_ast* ast, size_t n)
{
    const jet_token* tok = jet_ast_peekn_tok(ast, n);
    if(tok == NULL)
        return TOK_EOF;
    return tok->type;
}

static const char* jet_ast_get_type_name(jet_token_type tok_type)
{
    switch(tok_type)
    {
        default:
        {
            fprintf(stderr, "error: token type (enum-id: %d) is not recognized as a native type.\n", (int)tok_type);
            return "invalid";
        }
        case TOK_KWD_INT: return "int";
        case TOK_KWD_FLOAT: return "float";
        case TOK_KWD_STR: return "str";
        case TOK_KWD_BOOL: return "bool";
        case TOK_KWD_CHAR: return "char";
        case TOK_KWD_VOID: return "void";
    }
}

// PARSING ==============================================================================

static bool jet_ast_is_type_tok(jet_token_type tok_type)
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

static bool jet_ast_is_vdecl(jet_ast* ast)
{
    jet_token_type t = jet_ast_peekn_tok_type(ast, 0);
    if(!jet_ast_is_type_tok(t))
        return false;
    t = jet_ast_peekn_tok_type(ast, 1);
    if(t != TOK_IDENT)
        return false;
    t = jet_ast_peekn_tok_type(ast, 2);
    if(t != TOK_ASG && t != TOK_SEMI)
        return false;
    return true;
}

static bool jet_ast_is_func_head(jet_ast* ast)
{
    jet_token_type t = jet_ast_peekn_tok_type(ast, 0);
    if(!jet_ast_is_type_tok(t))
        return false;
    t = jet_ast_peekn_tok_type(ast, 1);
    if(t != TOK_IDENT) 
        return false;
    t = jet_ast_peekn_tok_type(ast, 2);
    if(t != TOK_LPAR)
        return false;
    return true;
}

static node_id jet_astn_parse_next_stmt(jet_ast* ast)
{
    if(!ast)
    {
        fprintf(stderr, "error: cannot parse next stmt, ast is null.\n");
        return INVALID_NID;
    }
    
    node_id parsed_nid = INVALID_NID;
    jet_token_type t = jet_ast_peekn_tok_type(ast, 0);
    
    if(t == TOK_EOF)
    {
        printf("* ast parsing complete, end of file reached.\n");
        return INVALID_NID;
    }
    else if(t == TOK_INV)
    {
        fprintf(stderr, "error: cannot parse next stmt, invalid token.\n");
        return INVALID_NID; 
    }    
    else if(t == TOK_KWD_PROG)
    {
        parsed_nid = jet_astn_prog_parse(ast);
        if(parsed_nid == INVALID_NID)
        {
            fprintf(stderr, "error: unable to parse next stmt (prog)\n");
            return INVALID_NID;
        }
    }
    else if(jet_ast_is_vdecl(ast))
    {
        parsed_nid = jet_astn_vdecl_parse(ast);
        if(parsed_nid == INVALID_NID)
        {
            fprintf(stderr, "error: cannot parse next stmt (vdecl)\n");
            return INVALID_NID;
        }
    }
    else if(jet_ast_is_func_head(ast))
    { 
        parsed_nid = jet_astn_func_parse(ast);
        if(parsed_nid == INVALID_NID)
        {
            fprintf(stderr, "error: cannot parse next stmt (func)\n");
            return INVALID_NID;
        }
    }
    else 
    {
        parsed_nid = jet_astn_parse_expr_stmt(ast);
        if(parsed_nid == INVALID_NID)
        {
            fprintf(stderr, "cannot parse next stmt, expected expression statement.\n");
            return INVALID_NID;
        }
    }

    if(parsed_nid == INVALID_NID)
        fprintf(stderr, "error: unable to parse next stmt, no valid stmt sequences parsed.\n");

    return parsed_nid;
}

static node_id jet_astn_parse_expr_stmt(jet_ast* ast)
{
    node_id expr_nid = jet_astn_parse_expr(ast, 0);
    if(expr_nid == INVALID_NID)
    {
        fprintf(stderr, "error: cannot parse expr stmt, expected expr node.\n");
        return INVALID_NID;
    }
    if(jet_ast_expect_tok(ast, TOK_SEMI) == INVALID_NID)
    {
        fprintf(stderr, "err: cannot parse expr_stmt, expected TOK_SEMI.\n");
        return INVALID_NID;
    }
    return expr_nid;
}

static node_id jet_astn_prog_parse(jet_ast* ast)
{
    if(jet_ast_expect_tok(ast, TOK_KWD_PROG) == INVALID_NID)
    {
        fprintf(stderr, "err: cannot parse prog, expected TOK_KWD_PROG\n");
        return INVALID_NID;
    }
    
    jet_ast_node_prog prog;
    prog.block_nid = jet_astn_block_parse(ast);

    if(prog.block_nid == INVALID_NID)
    {
        fprintf(stderr, "error: unable to parse prog block.\n");
        return INVALID_NID;
    }
 
    jet_ast_node node;
    node.node_type = AST_PROG;
    node.as.prog = prog;
    return jet_ast_register_node(ast, (const jet_ast_node*)&node);
}

static node_id jet_astn_block_parse(jet_ast* ast)
{
    if(jet_ast_expect_tok(ast, TOK_LBRC) == INVALID_NID)
    {
        fprintf(stderr, "err: cannot parse block, expected TOK_LBRC.\n");
        return INVALID_NID;
    }
    jet_ast_node_block block = {0};

    if(!jet_da_init(&block.stmt_nid_da, 4, sizeof(node_id)))
    {
        fprintf(stderr, "err: cannot parse block, could not init block.stmt_nid_da.\n");
        return INVALID_NID;
    }

    node_id stmt_nid = INVALID_NID;
    jet_token_type t = TOK_EOF;
    while(true)
    {
        t = jet_ast_peekn_tok_type(ast, 0);
        if(t == TOK_EOF || t == TOK_INV)
        {
            fprintf(stderr, "err: cannot parse block, unexpected token.\n");
            goto fail;
        }
        else if(t == TOK_RBRC)
            break;

        stmt_nid = jet_astn_parse_next_stmt(ast);
        if(stmt_nid == INVALID_NID)
        {
            fprintf(stderr, "err: cannot parse block, unable to parse next stmt.\n");
            goto fail;
        }
        jet_da_append(&block.stmt_nid_da, (const void*)&stmt_nid);
    }
    if(jet_ast_expect_tok(ast, TOK_RBRC) == INVALID_NID)
    {
        fprintf(stderr, "err: cannot parse block, expected TOK_RBRC.\n");
        return INVALID_NID;
    }

   
    jet_ast_node node;
    node.node_type = AST_BLOCK;
    node.as.block = block;  
    return jet_ast_register_node(ast, (const jet_ast_node*)&node);

fail:
    jet_da_dispose(&block.stmt_nid_da);
    return INVALID_NID;
}

static node_id jet_astn_ident_parse(jet_ast* ast)
{
    const jet_token* tok = jet_ast_peek_tok(ast);
    if(!tok)
    {
        fprintf(stderr, "error: cannot parse ident, no valid tokens to peek.\n");
        return INVALID_NID;
    }
    if(tok->type != TOK_IDENT)
    {
        fprintf(stderr, "error: cannot parse ident, token type mismatch.\n");
        return INVALID_NID;
    }

    jet_ast_node_ident ident;
    ident.str = (const char*)jet_token_strdup(tok);
    if(!ident.str)
    {
        fprintf(stderr, "error: cannot parse ident, unable to create token string dup.\n");
        return INVALID_NID;
    }
    
    jet_ast_consume_tok(ast);
    
    jet_ast_node node;
    node.node_type = AST_IDENT;
    node.as.ident = ident;

    return jet_ast_register_node(ast, (const jet_ast_node*)&node);
}

static node_id jet_astn_lit_parse(jet_ast* ast)
{
    assert(ast != NULL);
    const jet_token* tok = jet_ast_peek_tok(ast);
    if(!tok)
    {
        fpritnf(stderr, "err: cannot parse lit, unable to peek tok.\n");
        return INVALID_NID;
    }
    jet_ast_node_lit lit;
    lit.lit_type = tok->type;
    switch(lit.lit_type)
    {
        default:
            fprintf(stderr, "error: could not parse lit, unrecognized lit_type (tok_type:%d).\n", (int)lit.lit_type);
            return INVALID_NID;
        case TOK_KWD_NULL:
        {
            lit.as.v = NULL;
            break;
        }
        case TOK_KWD_TRUE:
        {
            lit.as.b = true;
            break;
        }
        case TOK_KWD_FALSE:
        {
            lit.as.b = false;
            break;
        }
        case TOK_LIT_INT:
        {
           lit.as.i = jet_conv_stoi(tok->source + tok->origin, tok->len); 
           break;
        }
        case TOK_LIT_FLOAT:
        {
            lit.as.f = jet_conv_stof(tok->source + tok->origin, tok->len);
            break;
        }
        case TOK_LIT_CHAR:
        {
            lit.as.c = *(tok->source + tok->origin);
            break;
        }
        case TOK_LIT_STR:
        {
            lit.as.s = jet_token_strdup(tok); 
            break;
        }
    }
    jet_ast_consume_tok(ast);
    jet_ast_node node;
    node.node_type = AST_LIT;
    node.as.lit = lit;
    return jet_ast_register_node(ast, (const jet_ast_node*)&node);
}

static node_id jet_astn_tdecl_parse(jet_ast* ast)
{
    const jet_token* tok = jet_ast_consume_tok(ast);

    jet_ast_node_tdecl tdecl;
    tdecl.tname = jet_ast_get_type_name(tok->type);
    tdecl.byte_size = 4;
    tdecl.is_native = strcmp(tdecl.tname, "invalid" ) != 0;

    jet_ast_node node;
    node.node_type = AST_TYPE_DECL;
    node.as.tdecl = tdecl;

    return jet_ast_register_node(ast, (const jet_ast_node*)&node);
}

static node_id jet_astn_vdecl_parse(jet_ast* ast) 
{
    jet_ast_node_vdecl vdecl;
    vdecl.tdecl_nid = jet_astn_tdecl_parse(ast);
    if(vdecl.tdecl_nid == INVALID_NID)
    {
        fprintf(stderr, "err: cannot parse vdecl, expected tdecl.\n");
        return INVALID_NID;
    }
    
    vdecl.ident_nid = jet_astn_ident_parse(ast);
    if(vdecl.ident_nid == INVALID_NID)
    {
        fprintf(stderr, "err: cannot parse vdecl, expected ident.\n");
        return INVALID_NID;
    }

    vdecl.init_value_nid = INVALID_NID;
    jet_token_type tok_type = jet_ast_peekn_tok_type(ast, 0);
    if(tok_type == TOK_ASG)
    {
        jet_ast_consume_tok(ast);    
        vdecl.init_value_nid = jet_astn_parse_expr(ast, 0);
        if(vdecl.init_value_nid == INVALID_NID)
        {
            fprintf(stderr, "error: cannot parse vdecl, expected expr value after asg operator.\n");
            return INVALID_NID;
        } 
    }
     
    if(jet_ast_expect_tok(ast, TOK_SEMI) == INVALID_NID)
    {
        fprintf(stderr, "err: cannot parse, expected TOK_SEMI.\n");
        return INVALID_NID;
    }
    
    jet_ast_node node;
    node.node_type = AST_VAR_DECL;
    node.as.vdecl = vdecl;
    return jet_ast_register_node(ast, (const jet_ast_node*)&node);
}

static node_id jet_astn_func_parse(jet_ast* ast) 
{
    jet_ast_node_fdecl fdecl = {0};

    node_id ret_tdecl_nid = jet_astn_tdecl_parse(ast); 
    fdecl.ident_nid = jet_astn_ident_parse(ast);
    
    if( !jet_da_init(&fdecl.ret_tdecl_nid_da, 1, sizeof(node_id)) )
    {
        fprintf(stderr, "err: cannot parse func, unable to init fdecl.ret_tdecl_nid_da.\n");
        return INVALID_NID;
    }
    jet_da_append(&fdecl.ret_tdecl_nid_da, (const void*)&ret_tdecl_nid);

    if(jet_ast_expect_tok(ast, TOK_LPAR) == INVALID_NID)
    {
        fprintf(stderr, "err: cannot parse, expected TOK_LPAR.\n");
        jet_da_dispose(&fdecl.ret_tdecl_nid_da);
        return INVALID_NID;
    }

    if( !jet_da_init(&fdecl.param_nid_da, 2, sizeof(node_id)) )
    {
        fprintf(stderr, "err: cannot parse func, unable to init fdecl.param_nid_da.\n");
        jet_da_dispose(&fdecl.ret_tdecl_nid_da);
        return INVALID_NID;
    }

    node_id vdecl_nid = INVALID_NID;
    jet_token_type t = TOK_EOF;
    while(jet_ast_peekn_tok_type(ast, 0) != TOK_RPAR && jet_ast_peekn_tok_type(ast, 0) != TOK_EOF)
    {
        vdecl_nid = jet_astn_parse_fparam(ast);
        if(vdecl_nid == INVALID_NID)
        {
            fprintf(stderr, "error: cannot parse func, unable to parse parameter.\n");
            goto fail;
        }
        jet_da_append(&fdecl.param_nid_da, (const void*)&vdecl_nid);
        
        t = jet_ast_peekn_tok_type(ast, 0);
        switch(t)
        {
            case TOK_RPAR:
                break;
            case TOK_COMMA:
                jet_ast_consume_tok(ast);
                break;
            case TOK_EOF:
                fprintf(stderr, "error: cannot parse func, EOF reached.\n");
                goto fail;
            case TOK_INV:
                fprintf(stderr, "error: cannot parse func, invalid token encountered.\n");
                goto fail;
            default:
                fprintf(stderr, "error: cannot parse func, unexpected token (type-enum-id: %d) encountered.\n", (int)t);
                goto fail;
        }
    }

    if(jet_ast_expect_tok(ast, TOK_RPAR) == INVALID_NID)
    {
        fprintf(stderr, "err: cannot parse func, expected TOK_RPAR.\n");
        goto fail;
    }

    jet_ast_node fdecl_base;
    fdecl_base.node_type = AST_FUNC_DECL;
    fdecl_base.as.fdecl = fdecl;
    node_id out_func_nid = INVALID_NID;

    if(jet_ast_peekn_tok_type(ast, 0) == TOK_LBRC)
    {
        jet_ast_node_fdef fdef;
        fdef.block_nid = jet_astn_block_parse(ast);
        if(fdef.block_nid == INVALID_NID)
        {
            fprintf(stderr, "error: cannot parse func, unable to parse func definiton block.\n");
            goto fail;
        }
        fdef.fdecl_nid = jet_ast_register_node(ast, (const jet_ast_node*)&fdecl_base);
        
        jet_ast_node fdef_base;
        fdef_base.node_type = AST_FUNC_DEF;
        fdef_base.as.fdef = fdef;
        out_func_nid = jet_ast_register_node(ast, (const jet_ast_node*)&fdef_base); 
    }
    else
    {
        out_func_nid = jet_ast_register_node(ast, (const jet_ast_node*)&fdecl_base);
    }
    return out_func_nid;

fail:
    jet_da_dispose(&fdecl.ret_tdecl_nid_da);
    jet_da_dispose(&fdecl.param_nid_da);
    return INVALID_NID;
}

static node_id jet_astn_parse_fparam(jet_ast* ast)
{
    jet_ast_node_vdecl vdecl;
    vdecl.tdecl_nid = jet_astn_tdecl_parse(ast);
    if(vdecl.tdecl_nid == INVALID_NID)
    {
        fprintf(stderr, "error: cannot parse vdecl, expected tdecl.\n");
        return INVALID_NID;
    }

    vdecl.ident_nid = jet_astn_ident_parse(ast);
    if(vdecl.ident_nid == INVALID_NID)
    {
        fprintf(stderr, "error: cannot parse vdecl, expected ident after tdecl.\n");
        return INVALID_NID;
    }

    vdecl.init_value_nid = INVALID_NID;
    jet_token_type tok_type = jet_ast_peekn_tok_type(ast, 0);
    if(tok_type == TOK_ASG)
    {
        jet_ast_consume_tok(ast);    
        vdecl.init_value_nid = jet_astn_parse_expr(ast, 0);
        if(!vdecl.init_value_nid)
        {
            fprintf(stderr, "error: cannot parse vdecl, expected expr value after asg operator.\n");
            return INVALID_NID;
        } 
    }
    
    jet_ast_node node;
    node.node_type = AST_VAR_DECL;
    node.as.vdecl = vdecl;
    return jet_ast_register_node(ast, (const jet_ast_node*)&node);
}

static node_id jet_astn_parse_expr(jet_ast* ast, size_t min_prec)
{
    node_id lhs_nid = jet_astn_parse_primary(ast);
    if(lhs_nid == INVALID_NID)
    {
        fprintf(stderr, "wrn: cannot parse expression, expected primary lhs.\n");
        return INVALID_NID;
    }
    while(jet_ast_peek_tok(ast) != NULL)
    {
        const jet_token* op_tok = jet_ast_peek_tok(ast);
        if(op_tok == NULL)
            break;
        size_t op_prec = jet_ast_get_op_prec(op_tok->type);
        if(op_prec == 0)
            break;
        if(op_prec < min_prec)
            break;
        jet_ast_consume_tok(ast);
        node_id rhs_nid = jet_astn_parse_expr(ast, op_prec + 1);        
        if(rhs_nid == INVALID_NID)
        {
            fprintf(stderr, "error: cannot parse expression, expected rhs expr after operator.\n");
            return INVALID_NID;
        }
        
        jet_ast_node_binop binop;
        binop.lhs_nid = lhs_nid;
        binop.rhs_nid = rhs_nid;
        binop.op_type = op_tok->type;
        
        jet_ast_node node;
        node.node_type = AST_BINOP;
        node.as.binop = binop;
        lhs_nid = jet_ast_register_node(ast, (const jet_ast_node*)&node);
    }
    return lhs_nid;
}

static node_id jet_astn_parse_primary(jet_ast* ast)
{
    const jet_token* cur_tok = jet_ast_peek_tok(ast);
    if(cur_tok == NULL) 
        return INVALID_NID;

    node_id out_nid = INVALID_NID;
    switch(cur_tok->type)
    {
        default:
        {
            fprintf(stderr, "error: expected primary expression.\n");
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
            out_nid = jet_astn_lit_parse(ast);
            break;
        }
        case TOK_IDENT:
        {
            out_nid = jet_astn_ident_parse(ast);
            if(out_nid == INVALID_NID)
            {
                fprintf(stderr, "error: cannot parse primary, unable to parse ident.\n");
                return INVALID_NID;
            }
            break;
        }
        case TOK_LPAR:
        {
            jet_ast_consume_tok(ast);
            out_nid = jet_astn_parse_expr(ast, 0);
            cur_tok = jet_ast_peek_tok(ast);
            if(cur_tok == NULL || cur_tok->type != TOK_RPAR)
            {
                fprintf(stderr, "error: expected ')' after primary expression.\n");
                return INVALID_NID;
            }
            jet_ast_consume_tok(ast);
            break;
        }
        case TOK_NOT:
        case TOK_MINUS:
        {
            jet_ast_consume_tok(ast);
            node_id rhs_nid = jet_astn_parse_expr(ast, PREC_PREFIX);
            if(rhs_nid == INVALID_NID)
            {
                fprintf(stderr, "error: expected expr after unary operator\n");
                return INVALID_NID;
            }
            jet_ast_node_unop unop;
            unop.op_type = cur_tok->type;
            unop.expr_nid = rhs_nid;

            jet_ast_node node;
            node.node_type = AST_UNOP;
            node.as.unop = unop;
            out_nid = jet_ast_register_node(ast, (const jet_ast_node*)&node);
            break;
        }
    }  

    if(out_nid == INVALID_NID)
    {
        fprintf(stderr, "error: no valid primary expressions parsed.\n");
        return INVALID_NID;
    }

    //postfix expression call evaluation
    while(jet_ast_peekn_tok_type(ast, 0) == TOK_LPAR)
    {
        jet_da arg_da = {0};
        if( !jet_da_init(&arg_da, 2, sizeof(node_id)) )
        {
            fprintf(stderr, "err: cannot parse primary expr, unable to init arg_da.\n");
            return INVALID_NID;
        }
        jet_ast_consume_tok(ast);        

        while(jet_ast_peekn_tok_type(ast, 0) != TOK_RPAR)
        {
            node_id arg_nid = jet_astn_parse_expr(ast, 0);
            if(arg_nid == INVALID_NID)
            {
                fprintf(stderr, "error: cannot parse primary expr, unable to parse call arg.\n");
                jet_da_dispose(&arg_da);
                return INVALID_NID;
            }
            if(!jet_da_append(&arg_da, (const void*)&arg_nid))
            {
                fprintf(stderr, "err: cannot parse primary expr, unable append to arg_da.\n");
                jet_da_dispose(&arg_da);
                return INVALID_NID;
            }

            if(jet_ast_peekn_tok_type(ast, 0) == TOK_COMMA)
                jet_ast_consume_tok(ast);
            else break;
        }
        
        if(jet_ast_expect_tok(ast, TOK_RPAR) == INVALID_NID)
        {
            fprintf(stderr, "err: cannot parse, expected TOK_RPAR.\n");
            jet_da_dispose(&arg_da);
            return INVALID_NID;
        }

        if(jet_da_is_empty(&arg_da))
            jet_da_dispose(&arg_da);
        
        jet_ast_node_call call;
        call.callee_nid = out_nid;
        call.arg_nid_da = arg_da;

        jet_ast_node call_base;
        call_base.node_type = AST_CALL;
        call_base.as.call = call;
        out_nid = jet_ast_register_node(ast, (const jet_ast_node*)&call_base);
    }
    return out_nid;
}


















