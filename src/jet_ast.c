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

struct jet_ast 
{
    jet_da* node_registry;
    jet_da* tok_da;
    jet_da* top_nid_da;

    node_id prog_nid;
    size_t node_count;
    size_t tok_cursor;
};

static node_id jet_ast_register_node(jet_ast* ast, const jet_ast_node* node);
static jet_ast_node* jet_ast_node_get(jet_ast* ast, node_id nid);
static void jet_ast_reset(jet_ast* ast);

// TOKEN TRAVERSING/UTILITY
static void jet_ast_push_nid(jet_ast* ast, node_id nid);
static jet_token* jet_ast_peek_tok(jet_ast* ast);
static jet_token* jet_ast_peekn_tok(jet_ast* ast, size_t n);
static jet_token* jet_ast_consume_tok(jet_ast* ast);
static jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type);
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

static node_id jet_astn_func_parse(jet_ast* ast);
static node_id jet_astn_parse_fparam(jet_ast* ast);


jet_ast* jet_ast_create(jet_da* tok_da)
{    
    if(!tok_da)
    {
        fprintf(stderr, "error: cannot create ast, invalid token darray.\n");
        return NULL;
    }
    
    jet_ast* ast = malloc(sizeof(jet_ast));
    
    if(!ast) 
        goto fail;
    
    ast->node_registry = NULL;
    ast->top_nid_da = NULL;
    ast->prog_nid = INVALID_NID;
    ast->tok_cursor = 0;
    ast->node_count = 0;
    
    ast->node_registry = jet_da_create(64, sizeof(jet_ast_node));
    if(!ast->node_registry) 
        goto fail;

    ast->top_nid_da = jet_da_create(32, sizeof(node_id));
    if(!ast->top_nid_da)
        goto fail;

    return ast;

fail:
    if(ast)
    {
        if(ast->node_registry) jet_da_dispose(ast->node_registry);
        if(ast->top_nid_da) jet_da_dispose(ast->top_nid_da);
        free(ast);
        fprintf(stderr, "err: ast data couldn't be initialized.\n");
    }
    else
    {
        fprintf(stderr, "err: ast memory couldn't be allocated\n");
    }
    return NULL;
}


bool jet_ast_dispose(jet_ast* ast)
{
    if(!ast)
    {
        fprintf(stderr, "error: cannot dispose, given jet_ast ptr is NULL.\n");
        return false;
    }

    if(ast->node_registry)
        jet_da_dispose(ast->node_registry);
    if(ast->top_nid_da)
        jet_da_dispose(ast->top_nid_da);
    free(ast);
    return true;
}

bool jet_ast_generate_nodes(jet_ast* ast)
{
    if(ast == NULL)
    {
        fprintf(stderr, "error: cannot generate ast nodes, given ast is NULL.\n");
        return false;
    }

    if(ast->tok_da == NULL)
    {
        fprintf(stderr, "error: cannot generate ast nodes, ast was not initialized with valid token darray.\n");
        return false;
    }

    if(ast->node_registry == NULL)
    {
        fprintf(stderr, "err: cannot generate ast nodes, node_registry was not initialized.\n");
        return false;
    }
    
    if(ast->top_nid_da == NULL)
    {
        fprintf(stderr, "error: cannot generate ast nodes, ast did not initialize top_nid_da.\n");
        return false;
    }

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

static node_id jet_ast_register_node(jet_ast* ast, const jet_ast_node* node)
{
    assert(ast != NULL);
    assert(node != NULL);

    jet_da_append(ast->node_registry, (const void*)node);
    ast->node_count++;
    return ast->node_count;
}

static jet_ast_node* jet_ast_node_get(jet_ast* ast, node_id nid)
{
    assert(ast != NULL);
    if(nid == INVALID_NID)
    {
        fprintf(stderr, "err: cannot get node with id=%zu, %zu internally represents invalid.\n", INVALID_NID);
        return NULL;
    }

    // force nid - 1 >= 0, because 0 represents invalid.
    jet_ast_node* node = (jet_ast_node*)jet_da_get(ast->node_registry, nid - 1);
    if(!node)
    {
        fprintf(stderr, "err: node with id=%zu does not exist in registry.\n", nid);
        return NULL;
    }
    return node;
}

static void jet_ast_reset(jet_ast* ast)
{
    assert(ast != NULL); 
    jet_da_clear(ast->node_registry);
    jet_da_clear(ast->top_nid_da);  
    ast->tok_cursor = 0;
    ast->node_count = 0;
    ast->prog_nid = INVALID_NID;
}

static void jet_ast_push_nid(jet_ast* ast, node_id nid)
{
    jet_ast_node* node = jet_astn_get(nid);
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
    else jet_da_append(ast->top_nid_da, nid);
}

static jet_token* jet_ast_peek_tok(jet_ast* ast)
{
    return jet_ast_peekn_tok(ast, 0);
}

static jet_token* jet_ast_peekn_tok(jet_ast* ast, size_t n)
{ 
    assert(ast != NULL);
    assert(ast->tok_da != NULL);
    size_t count = jet_da_count(ast->tok_da);
    if(count <= 0)
    {
        fprintf(stderr, "wrn: ast->tok_da is empty.\n");        
        return NULL; 
    }
    else if(ast->tok_cursor + n >= count)
    {
        fprintf(stderr, "wrn: cannot peak ahead %zu tokens (cursor=%zu), index out of bounds.\n", n, ast->tok_cursor);
        return NULL;
    }
    return (jet_token*)jet_da_get(ast->tok_da, ast->tok_cursor + n);
}

static jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type)
{
    assert(ast != NULL);
    assert(ast->tok_da != NULL);
    if(ast->tok_cursor >= jet_da_count(ast->tok_da))
    {
        puts("end of token-darray reached.");
        return NULL;
    }
    jet_token* tok = jet_ast_peek_tok(ast);
    assert(tok != NULL);
    
    if(tok->type != tok_type)
    {
        fprintf(stderr, "error: expected token type (id: %d) but encountered (id: %d).\n", (int)tok_type, (int)tok->type);
        return NULL;
    }
    return jet_ast_consume_tok(ast);
}

static jet_token* jet_ast_consume_tok(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_da != NULL);
    if(ast->tok_cursor >= jet_da_count(ast->tok_da))
    {
        puts("- cannot consume, end of token-darray reached.");
        return NULL;
    }
    ast->tok_cursor++;
    return (jet_token*)jet_da_get(ast->tok_da , ast->tok_cursor - 1);
}

static jet_token_type jet_ast_peekn_tok_type(jet_ast* ast, size_t n)
{
    jet_token* tok = jet_ast_peekn_tok(ast, n);
    if(tok == NULL)
        return TOK_EOF;
    return tok->type;
}

static const char* jet_ast_get_type_name(jet_token_type tok_type)
{
    const char* type_name;
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
    return type_name;
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
    node_id expr = jet_astn_parse_expr(ast, 0);
    if(expr == INVALID_NID)
    {
        fprintf(stderr, "error: cannot parse expr stmt, expected expr node.\n");
        return INVALID_NID;
    }
    jet_ast_expect_tok(ast, TOK_SEMI);
    return expr;
}

static node_id jet_astn_prog_parse(jet_ast* ast)
{
    jet_ast_expect_tok(ast, TOK_KWD_PROG);
    node_id block = jet_astn_block_parse(ast);
    if(block == INVALID_NID)
    {
        fprintf(stderr, "error: unable to parse prog block.\n");
        return INVALID_NID;
    }

    jet_ast_node_prog prog;
    prog.block_nid = block;
    
    jet_ast_node node;
    node.node_type = AST_PROG;
    node.as.prog = prog;

    return jet_ast_register_node((const jet_ast_node*)&node);
}

//WOKRING...
static node_id jet_astn_block_parse(jet_ast* ast)
{
    jet_ast_expect_tok(ast, TOK_LBRC);
    jet_da* stmt_da = jet_da_create(4, sizeof(node_id));
    if(!stmt_da)
    {
        fprintf(stderr, "error: cannot parse block, could not create node da.\n");
        return INVALID_NID;
    }
    while(jet_ast_peekn_tok_type(ast, 0) != TOK_RBRC)
    {
        node_id stmt = jet_astn_parse_next_stmt(ast);
        if(stmt == INVALID_NID)
        {
            fprintf(stderr, "error: cannot parse block, unable to parse next stmt.\n");
            return INVALID_NID;
        }
        jet_da_append(stmt_da, (const void*)stmt);
    }
    jet_ast_expect_tok(ast, TOK_RBRC);

    jet_ast_node_block block;
    block.stmt_nid_da = stmt_da;
    
    jet_ast_node node;
    node.node_type = AST_BLOCK;
    node.as.block = block;
    
    return jet_ast_register_node((const jet_ast_node*)&node);
}

static jet_ast_node* jet_astn_ident_parse(jet_ast* ast)
{
    jet_token* tok = jet_ast_peek_tok(ast);
    if(!tok)
    {
        fprintf(stderr, "error: cannot parse ident, no valid tokens to peek.\n");
        return NULL;
    }
    if(tok->type != TOK_IDENT)
    {
        fprintf(stderr, "error: cannot parse ident, token type mismatch.\n");
        return NULL;
    }
    const char* ident_str = (const char*)jet_token_strdup(tok);
    if(!ident_str)
    {
        fprintf(stderr, "error: cannot parse ident, unable to create token string dup.\n");
        return NULL;
    }
    jet_ast_node* ident = jet_astn_ident_create(ident_str);
    jet_ast_consume_tok(ast);
    return ident;
}

static jet_ast_node* jet_astn_tdecl_parse(jet_ast* ast)
{
    jet_token* tok = jet_ast_consume_tok(ast);
    const char* tname = jet_ast_get_type_name(tok->type);
    size_t size = 4;
    bool is_native = strcmp(tname, "invalid") != 0;
    jet_ast_node* tdecl = jet_astn_tdecl_create(tname, size, is_native); 
    return tdecl;
}

static jet_ast_node* jet_astn_vdecl_parse(jet_ast* ast) 
{
    jet_ast_node* tdecl = jet_astn_tdecl_parse(ast);
    if(!tdecl)
    {
        fprintf(stderr, "error: cannot parse vdecl, expected tdecl.\n");
        return NULL;
    }

    jet_ast_node* ident = jet_astn_ident_parse(ast);
    if(!ident)
    {
        fprintf(stderr, "error: cannot parse vdecl, expected ident after tdecl.\n");
        return NULL;
    }

    jet_ast_node* init = NULL;
    jet_token_type tok_type = jet_ast_peekn_tok_type(ast, 0);
    if(tok_type == TOK_ASG)
    {
        jet_ast_consume_tok(ast);    
        init = jet_astn_parse_expr(ast, 0);
        if(!init)
        {
            fprintf(stderr, "error: cannot parse vdecl, expected expr value after asg operator.\n");
            return NULL;
        } 
    }
    jet_ast_expect_tok(ast, TOK_SEMI);
    jet_ast_node* vdecl = jet_astn_vdecl_create(tdecl, ident, init);
    return vdecl;
}

static jet_ast_node* jet_astn_func_parse(jet_ast* ast) 
{
    jet_ast_node* tdecl = jet_astn_tdecl_parse(ast);
    jet_ast_node* ident = jet_astn_ident_parse(ast);
    jet_ast_expect_tok(ast, TOK_LPAR);
    jet_da* param_darray = jet_da_create(4, sizeof(jet_ast_node));
    if(!param_darray)
    {
        fprintf(stderr, "error: cannot parse func, unable to create param darray.\n");
        return NULL;
    }

    jet_ast_node* vdecl = NULL;
    jet_token_type t = TOK_EOF;
    while(jet_ast_peekn_tok_type(ast, 0) != TOK_RPAR)
    {
        vdecl = jet_astn_parse_fparam(ast);
        if(vdecl == NULL)
        {
            fprintf(stderr, "error: cannot parse func, unable to parse parameter.\n");
            return NULL;
        }
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
                return NULL;
            case TOK_INV:
                fprintf(stderr, "error: cannot parse func, invalid token encountered.\n");
                return NULL;
            default:
                fprintf(stderr, "error: cannot parse func, unexpected token (type-enum-id: %d) encountered.\n", (int)t);
                return NULL;
        }
        jet_da_append(param_darray, (const void*)vdecl);
    }
    jet_ast_expect_tok(ast, TOK_RPAR); 
    if(jet_da_is_empty(param_darray))
    {
        jet_da_dispose(param_darray);
        param_darray = NULL;
    }

    jet_da* ret_type_darray = jet_da_create(1, sizeof(jet_ast_node));
    jet_da_append(ret_type_darray, (const void*)tdecl);
    
    jet_ast_node* func = jet_astn_fdecl_create(ident, ret_type_darray, param_darray);
    
    if(jet_ast_peekn_tok_type(ast, 0) == TOK_LBRC)
    {
        jet_ast_node* block = jet_astn_block_parse(ast);
        if(block == NULL)
        {
            fprintf(stderr, "error: cannot parse func, unable to parse func definiton block.\n");
            return NULL;
        }
        func = jet_astn_fdef_create(func, block);
    }
    return func;
}

static jet_ast_node* jet_astn_parse_fparam(jet_ast* ast)
{
    jet_ast_node* tdecl = jet_astn_tdecl_parse(ast);
    if(!tdecl)
    {
        fprintf(stderr, "error: cannot parse vdecl, expected tdecl.\n");
        return NULL;
    }

    jet_ast_node* ident = jet_astn_ident_parse(ast);
    if(!ident)
    {
        fprintf(stderr, "error: cannot parse vdecl, expected ident after tdecl.\n");
        return NULL;
    }

    jet_ast_node* init = NULL;
    jet_token_type tok_type = jet_ast_peekn_tok_type(ast, 0);
    if(tok_type == TOK_ASG)
    {
        jet_ast_consume_tok(ast);    
        init = jet_astn_parse_expr(ast, 0);
        if(!init)
        {
            fprintf(stderr, "error: cannot parse vdecl, expected expr value after asg operator.\n");
            return NULL;
        } 
    }
    jet_ast_node* vdecl = jet_astn_vdecl_create(tdecl, ident, init);
    return vdecl; 
}

static jet_ast_node* jet_astn_parse_expr(jet_ast* ast, size_t min_prec)
{
    jet_ast_node* lhs_node = jet_astn_parse_primary(ast);
    if(lhs_node == NULL)
    {
        fprintf(stderr, "wrn: cannot parse expression, expected primary lhs.\n");
        return NULL;
    }
    while(jet_ast_peek_tok(ast) != NULL)
    {
        jet_token* op_tok = jet_ast_peek_tok(ast);
        if(op_tok == NULL)
            break;
        size_t op_prec = jet_ast_get_op_prec(op_tok->type);
        if(op_prec == 0)
            break;
        if(op_prec < min_prec)
            break;
        jet_ast_consume_tok(ast);
        jet_ast_node* rhs_node = jet_astn_parse_expr(ast, op_prec + 1);        
        if(rhs_node == NULL)
        {
            fprintf(stderr, "wrn: cannot parse expression, expected rhs after operator.\n");
            return lhs_node;
        }
        lhs_node = jet_astn_binop_create(lhs_node, rhs_node, op_tok->type);
    }
    return lhs_node;
}

static jet_ast_node* jet_astn_parse_primary(jet_ast* ast)
{
    jet_token* cur_tok = jet_ast_peek_tok(ast);
    if(cur_tok == NULL) 
        return NULL;

    jet_ast_node* out_node = NULL;
    switch(cur_tok->type)
    {
        default:
        {
            fprintf(stderr, "error: expected primary expression.\n");
            return NULL;
        }
        case TOK_KWD_NULL:
        case TOK_KWD_TRUE:
        case TOK_KWD_FALSE:
        case TOK_LIT_INT:
        case TOK_LIT_FLOAT:
        case TOK_LIT_CHAR:
        case TOK_LIT_STR:
        {
            jet_ast_consume_tok(ast);
            out_node = jet_astn_lit_create(cur_tok);
            break;
        }
        case TOK_IDENT:
        {
            out_node = jet_astn_ident_parse(ast);
            if(!out_node)
            {
                fprintf(stderr, "error: cannot parse primary, unable to parse ident.\n");
                return NULL;
            }
            break;
        }
        case TOK_LPAR:
        {
            jet_ast_consume_tok(ast);
            out_node = jet_astn_parse_expr(ast, 0);
            cur_tok = jet_ast_peek_tok(ast);
            if(cur_tok == NULL || cur_tok->type != TOK_RPAR)
            {
                fprintf(stderr, "error: expected ')' after primary expression.\n");
                return NULL;
            }
            jet_ast_consume_tok(ast);
            break;
        }
        case TOK_NOT:
        case TOK_MINUS:
        {
            jet_ast_consume_tok(ast);
            jet_ast_node* rhs = jet_astn_parse_expr(ast, 0);
            if(rhs == NULL)
            {
                fprintf(stderr, "error: expected expr after '-'\n");
                return NULL;
            }
            out_node = jet_astn_unop_create(rhs, cur_tok->type);
            break;
        }
    }  

    if(out_node == NULL)
    {
        fprintf(stderr, "error: no valid primary expressions parsed.\n");
        return NULL;
    }

    //postfix expression call evaluation
    while(jet_ast_peekn_tok_type(ast, 0) == TOK_LPAR)
    {
        jet_ast_consume_tok(ast);        
        jet_da* arg_darray = jet_da_create(4, sizeof(jet_ast_node));

        while(jet_ast_peekn_tok_type(ast, 0) != TOK_RPAR)
        {
            jet_ast_node* arg = jet_astn_parse_expr(ast, 0);
            if(!arg)
            {
                fprintf(stderr, "error: cannot parse primary expr, unable to parse call arg.\n");
                return NULL;
            }
            jet_da_append(arg_darray, arg);

            if(jet_ast_peekn_tok_type(ast, 0) == TOK_COMMA)
                jet_ast_consume_tok(ast);
            else break;
        }
        jet_ast_expect_tok(ast, TOK_RPAR);
        if(jet_da_is_empty(arg_darray))
        {
            jet_da_dispose(arg_darray);
            arg_darray = NULL;
        }
        out_node = jet_astn_call_create(out_node, arg_darray);
    }
    return out_node;
}

// DEBUG ===================================================================================

void jet_ast_print(jet_ast* ast)
{
    if(!ast)
    {
        fprintf(stderr, "error: cannot print, given ast is null.\n");
        return;
    }
    printf("\n=======================\n");
    printf("TOP\n");
    jet_ast_node_darray_print(ast->top_nid_da, 0);
    
    printf("=======================\n");
    printf("PROG\n");
    jet_ast_node_print(ast->prog_nid, 0); 
    printf("=======================\n\n");
}


















