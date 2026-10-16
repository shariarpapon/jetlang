#include <jet_parser.h>
#include <jet_parser_ops.h>
#include <jet_ast.h>
#include <jet_ast_node.h>
#include <jet_conv.h>
#include <jet_sb.h>
#include <jet_logger.h>
#include <jet_result.h>

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define JET_PARSER_MAX_ERRORS (64)
#define ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

// TRAVERSING UTILS
static const jet_token* jet_parser_peek_prev_tok(jet_parser* p);
static const jet_token* jet_parser_peek_tok(jet_parser* p);
static const jet_token* jet_parser_peekn_tok(jet_parser* p, size_t n);
static const jet_token* jet_parser_consume_tok(jet_parser* p);
static const jet_token* jet_parser_expect_tok(jet_parser* p, jet_token_type tok_type);
static jet_token_type jet_parser_peekn_tok_type(jet_parser* p, size_t n);

// PARSING
static const char* jet_parser_get_type_name(jet_parser* p, const jet_token* tok);
static bool jet_parser_is_sequence_match(jet_parser* p, jet_token_type seq[], size_t len);
static bool jet_parser_is_vdecl(jet_parser* p);
static bool jet_parser_is_func_head(jet_parser* p);
static bool jet_parser_is_prog_entry(jet_parser* p);
static bool jet_parser_is_tdecl(jet_parser* p);
static bool jet_parser_is_tdef(jet_parser* p);
static bool jet_parser_is_user_defined_type(jet_parser* p, const jet_token* tok, char** out_name);

//abstract node parsing
static jet_result jet_parser_parse_next_toplevel_node(jet_parser* p);
static jet_result jet_parser_parse_expr_stmt(jet_parser* p);
static jet_result jet_parser_parse_expr(jet_parser* p, size_t min_prec);
static jet_result jet_parser_parse_primary(jet_parser* ast);
static jet_result jet_parser_parse_block_stmt(jet_parser* p);

//specific node parsing
static jet_result jet_parser_prog_parse(jet_parser* p);
static jet_result jet_parser_block_parse(jet_parser* p);
static jet_result jet_parser_ident_parse(jet_parser* p);
static jet_result jet_parser_tdecl_parse(jet_parser* p);
static jet_result jet_parser_tdef_parse(jet_parser* p);
static jet_result jet_parser_vdecl_parse(jet_parser* p);
static jet_result jet_parser_lit_parse(jet_parser* p);
static jet_result jet_parser_func_parse(jet_parser* p);
static jet_result jet_parser_parse_fparam(jet_parser* p);

static jet_span jet_parser_token_range_span(const jet_token* start, const jet_token* end);
static jet_result jet_parser_register_node(jet_parser* p, const jet_ast_node* node);
static jet_type_kind jet_parser_tkind_from_token(const jet_token* tok);
static void jet_parser_register_tdef_name(jet_parser* p, const char* name);

//error recovery and syncrhonization 
static jet_result jet_parser_result_parsing_failed(jet_ast_node_type node_type, const jet_token* tok);
static void jet_parser_push_err(jet_parser* p, const jet_error* err);
static bool jet_parser_sync_toplevel(jet_parser* p);
static bool jet_parser_sync_block(jet_parser* p);

// exposed fns
bool jet_parser_init(jet_parser* p, const char* filename, const jet_da* tok_da, jet_ast* ast, jet_da* err_da, jet_da* user_type_da)
{
    JET_ASSERT(p != NULL);
    if(!tok_da || !err_da || !user_type_da || !ast || !filename)
    {
        JET_LOG_ERROR("could not init parser, null param/s.");
        return false;
    }

    memset(p, 0 , sizeof(*p));
    p->filename = filename;
    p->tok_da = tok_da;
    p->err_da = err_da;
    p->user_type_da = user_type_da;
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
        jet_result result = jet_parser_parse_next_toplevel_node(p); 
        if(result.success == true)
            jet_ast_push_nid(p->ast, result.as.nid);
        else
        {
            jet_parser_push_err(p, (const jet_error*)&result.as.error);
            if(!jet_parser_sync_toplevel(p))
                return false;
        }
    }
    return true;
}

static void jet_parser_register_tdef_name(jet_parser* p, const char* name)
{
    JET_ASSERT(name != NULL);
    JET_ASSERT(jet_da_append(p->user_type_da, (const void*)&name));
}

static jet_result jet_parser_result_parsing_failed(jet_ast_node_type node_type, const jet_token* tok)
{
    jet_error err = jet_err_parsing_failed(node_type, tok);
    return jet_result_error((const jet_error*)&err);
}

static void jet_parser_push_err(jet_parser* p, const jet_error* err)
{
    JET_ASSERT(p != NULL);
    JET_ASSERT(err != NULL);
    JET_ASSERT(p->err_da != NULL);
    JET_ASSERT(jet_da_append(p->err_da, (const void*)err));
}

static jet_span jet_parser_token_range_span(const jet_token* start, const jet_token* end)
{
    jet_span span = 
    {
        .start = start->span.start,
        .end = end->span.end,
        .line = start->span.line,
        .col = start->span.col
    };
    return span;
}

static bool jet_parser_sync_toplevel(jet_parser* p)
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

static bool jet_parser_sync_block(jet_parser* p)
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

static jet_result jet_parser_register_node(jet_parser* p, const jet_ast_node* node)
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

static jet_type_kind jet_parser_tkind_from_token(const jet_token* tok)
{
    JET_ASSERT(tok != NULL);
    switch(tok->type)
    {
        default:             return JET_TYPE_INVALID;
        case TOK_IDENT:      return JET_TYPE_CUSTOM;
        case TOK_KWD_BOOL:   return JET_TYPE_BOOL;
        case TOK_KWD_INT:    return JET_TYPE_INT;
        case TOK_KWD_FLOAT:  return JET_TYPE_FLOAT;
        case TOK_KWD_STR:    return JET_TYPE_STR;
        case TOK_KWD_CHAR:   return JET_TYPE_CHAR;
    }
}

static const char* jet_parser_get_type_name(jet_parser* p, const jet_token* tok)
{
    JET_ASSERT(p != NULL);
    JET_ASSERT(tok != NULL);
    switch(tok->type)
    {
        default: return NULL;
        case TOK_IDENT: 
        {
            char* name;
            if(jet_parser_is_user_defined_type(p, tok, &name))
                return name;
            break;
        }
        case TOK_KWD_INT:   return "int";
        case TOK_KWD_FLOAT: return "float";
        case TOK_KWD_STR:   return "str";
        case TOK_KWD_BOOL:  return "bool";
        case TOK_KWD_CHAR:  return "char";
    }
}

// local definitions --- 
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

    //count > 0 is asserted at init
    if(n >= count - p->tok_cursor) 
        return (const jet_token*)jet_da_get(p->tok_da, count - 1);

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
    if(!tok)
    {
        jet_error err = jet_err_expected_token(jet_parser_peek_prev_tok(p), tok_type);
        jet_parser_push_err(p, (const jet_error*)&err);
        return NULL;
    }
    else if(tok->type != tok_type)
    {
        jet_error err = jet_err_expected_token(tok, tok_type);
        jet_parser_push_err(p, (const jet_error*)&err);
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

//parsing ---
static bool jet_parser_is_sequence_match(jet_parser* p, jet_token_type seq[], size_t len)
{
    for(size_t i = 0; i < len; i++)
    {
        if(seq[i] != jet_parser_peekn_tok_type(p, i))        
            return false;
    }
    return true;
}

static bool jet_parser_is_tdef(jet_parser* p)
{
    jet_token_type seq[] = { TOK_KWD_TYPE, TOK_IDENT, TOK_LBRC };
    return jet_parser_is_sequence_match(p, seq, ARRAY_LEN(seq));
}

static bool jet_parser_is_user_defined_type(jet_parser* p, const jet_token* tok, char** out_name)
{
    JET_ASSERT(tok != NULL);
    if(tok->type != TOK_IDENT) return false;
    char* name;
    jet_token_strdup(tok, &name);
    if(name == NULL) 
        return false;
    
    bool is_type = false;
    for(size_t i = 0; i < jet_da_count((const jet_da*)p->user_type_da); i++)
    {
        char* registered_name = *((char**)jet_da_get((const jet_da*)p->user_type_da, i));
        if(strcmp(registered_name, name) == 0)
        {
            is_type = true;
            if(out_name != NULL)
                *out_name = registered_name;
            break;
        }
    }
    free(name);
    return is_type;
}

static bool jet_parser_is_tdecl(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_peek_tok(p);
    switch(tok->type)
    {
        case TOK_KWD_INT:
        case TOK_KWD_FLOAT:
        case TOK_KWD_STR:
        case TOK_KWD_BOOL:
        case TOK_KWD_CHAR:
        case TOK_KWD_VOID:
            return true;
        case TOK_IDENT:
            return jet_parser_is_user_defined_type(p, tok, NULL);
        default:
            return false;
    }
}

static bool jet_parser_is_prog_entry(jet_parser* p)
{
    return jet_parser_peekn_tok_type(p, 0) == TOK_KWD_PROG;
}

static bool jet_parser_is_vdecl(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    return  jet_parser_is_tdecl(p) &&
            jet_parser_peekn_tok_type(p, 1) == TOK_IDENT && 
           (jet_parser_peekn_tok_type(p, 2) == TOK_ASG || 
            jet_parser_peekn_tok_type(p, 2) == TOK_SEMI);
}

static bool jet_parser_is_func_head(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    
    return jet_parser_is_tdecl(p) &&
           jet_parser_peekn_tok_type(p, 1) == TOK_IDENT &&
           jet_parser_peekn_tok_type(p, 2) == TOK_LPAR;
}

// parsing functions ------
static jet_result jet_parser_parse_next_toplevel_node(jet_parser* p)
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

static jet_result jet_parser_parse_block_stmt(jet_parser* p)
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


static jet_result jet_parser_parse_expr_stmt(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    jet_result result = jet_parser_parse_expr(p, 0);
    if(result.success)
        jet_parser_expect_tok(p, TOK_SEMI);
    return result;
}

static jet_result jet_parser_prog_parse(jet_parser* p)
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

static jet_result jet_parser_block_parse(jet_parser* p)
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

static jet_result jet_parser_ident_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL);
    const jet_token* tok = jet_parser_expect_tok(p, TOK_IDENT); 
    if(tok == NULL)
        return jet_parser_result_parsing_failed(AST_IDENT, jet_parser_peek_tok(p));

    char* ident_str;
    jet_token_strdup(tok, &ident_str);
    JET_ASSERT(ident_str != NULL);
    jet_ast_node node = jet_astn_ident(tok->span, ident_str);

    return jet_parser_register_node(p, (const jet_ast_node*)&node);
}

static jet_result jet_parser_lit_parse(jet_parser* p)
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

static jet_result jet_parser_tdef_parse(jet_parser* p)
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
    jet_span span = jet_parser_token_range_span(start_tok, end_tok);    

    char* name;
    jet_token_strdup(ident_tok, &name);
    JET_ASSERT(name != NULL);

    jet_parser_register_tdef_name(p, name);
    jet_ast_node node = jet_astn_tdef(span, name, block_result.as.nid);

    return jet_parser_register_node(p, (const jet_ast_node*)&node);
}

static jet_result jet_parser_tdecl_parse(jet_parser* p)
{
    JET_ASSERT(p != NULL); 
    if(!jet_parser_is_tdecl(p))
        goto fail;
    
    const jet_token* tok = jet_parser_consume_tok(p);
    jet_type_kind tkind = jet_parser_tkind_from_token(tok);
    char* tname;
    switch(tkind)
    {
        case JET_TYPE_INVALID:
            goto fail;
        default:
            tname = jet_parser_get_type_name(p, tok);
            JET_ASSERT(tname != NULL); 
            break;
    }
    jet_ast_node node = jet_astn_tdecl(tok->span, tkind, tname);
    return jet_parser_register_node(p, (const jet_ast_node*)&node);

fail:
    return jet_parser_result_parsing_failed(AST_TYPE_DECL, jet_parser_peek_tok(p));
}

static jet_result jet_parser_vdecl_parse(jet_parser* p) 
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);

    if(jet_parser_is_vdecl(p) == false)
        return jet_parser_result_parsing_failed(AST_VAR_DECL, start_tok);


    jet_ast_node_vdecl vdecl;
    vdecl.tdecl_nid = jet_parser_tdecl_parse(p);

    if(vdecl.tdecl_nid == INVALID_NID) {
        return INVALID_NID;
    }
    
    vdecl.ident_nid = jet_parser_ident_parse(p);

    if(vdecl.ident_nid == INVALID_NID) {
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
            return INVALID_NID;
        } 
    }
    
    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_SEMI);
    if(end_tok == NULL)
    {
        return INVALID_NID;
    }
    
    jet_ast_node node;
    JET_ASSERT(jet_ast_node_init(&node, AST_VAR_DECL, 
                start_tok->span.start, end_tok->span.end, 
                start_tok->span.line, start_tok->span.col));
    node.as.vdecl = vdecl;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static jet_result jet_parser_func_parse(jet_parser* p) 
{
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);
    jet_ast_node_fdecl fdecl;

    node_id ret_tdecl_nid = jet_parser_tdecl_parse(p); 

    fdecl.ident_nid = jet_parser_ident_parse(p);
    if(fdecl.ident_nid == INVALID_NID)
    {
        return INVALID_NID;
    }

    jet_parser_expect_tok(p, TOK_LPAR);
    
    JET_ASSERT(jet_da_init(&fdecl.ret_tdecl_nid_da, 1, sizeof(node_id)));
    JET_ASSERT(jet_da_append(&fdecl.ret_tdecl_nid_da, (const void*)&ret_tdecl_nid));
    JET_ASSERT(jet_da_init(&fdecl.param_nid_da, 2, sizeof(node_id)));

    node_id vdecl_nid = INVALID_NID;
    jet_token_type t = TOK_EOF;
    while(jet_parser_peekn_tok_type(p, 0) != TOK_RPAR && jet_parser_peekn_tok_type(p, 0) != TOK_EOF)
    {
        vdecl_nid = jet_parser_parse_fparam(p);

        if(vdecl_nid == INVALID_NID)
        {
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
            default:
                goto fail;
        }
    }

    const jet_token* end_tok = jet_parser_expect_tok(p, TOK_RPAR);
    if(end_tok == NULL)
    {
        goto fail;
    }

    jet_ast_node fdecl_base;
    JET_ASSERT(jet_ast_node_init(&fdecl_base, AST_FUNC_DECL, 
                start_tok->span.start, end_tok->span.end, 
                start_tok->span.line, start_tok->span.col));

    fdecl_base.as.fdecl = fdecl;
    node_id out_func_nid = INVALID_NID;
    if(jet_parser_peekn_tok_type(p, 0) == TOK_LBRC)
    {
        jet_ast_node_fdef fdef;
        fdef.block_nid = jet_parser_block_parse(p);
        if(fdef.block_nid == INVALID_NID)
        {
            goto fail;
        }
        fdef.fdecl_nid = jet_ast_register_node(p->ast, (const jet_ast_node*)&fdecl_base);
        end_tok = jet_parser_peek_prev_tok(p);
        if(!end_tok) end_tok = start_tok;

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

static jet_result jet_parser_parse_fparam(jet_parser* p)
{
   
    JET_ASSERT(p != NULL);
    const jet_token* start_tok = jet_parser_peek_tok(p);
    jet_ast_node_vdecl vdecl;
    vdecl.tdecl_nid = jet_parser_tdecl_parse(p);
     
    if(vdecl.tdecl_nid == INVALID_NID)
    {
        return INVALID_NID;
    }

    vdecl.ident_nid = jet_parser_ident_parse(p);
    if(vdecl.ident_nid == INVALID_NID)
    {
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
            return INVALID_NID;
        } 
    }
    const jet_token* end_tok = jet_parser_peek_prev_tok(p);  
    jet_ast_node node;
    JET_ASSERT(jet_ast_node_init(&node, AST_VAR_DECL, 
                start_tok->span.start, end_tok->span.end, 
                start_tok->span.line, start_tok->span.col));
    node.as.vdecl = vdecl;
    return jet_ast_register_node(p->ast, (const jet_ast_node*)&node);
}

static jet_result jet_parser_parse_expr(jet_parser* p, size_t min_prec)
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
        {
            out_nid = jet_parser_lit_parse(p);
            break;
        }
        case TOK_IDENT:
        {
            out_nid = jet_parser_ident_parse(p);
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

