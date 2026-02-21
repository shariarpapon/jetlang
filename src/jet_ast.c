/* 
 * NAMING CONVENTIONS
 *
 * peekn : peek_nth
 * astn  : ast_node 
 * tok   : token
 *
 * */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include <jet_ast.h>
#include <jet_ast_node.h>
#include <jet_ast_node_create.h>
#include <jet_ast_op_prec.h>

struct jet_ast 
{
    jet_list* tok_list;
    jet_list* top_node_list;
    jet_ast_node* prog_node;
    size_t tok_cursor;
};

// TOKEN TRAVERSING/UTILITY
static void jet_ast_push_node(jet_ast* ast, jet_ast_node* node);
static jet_token* jet_ast_peek_tok(jet_ast* ast);
static jet_token* jet_ast_peekn_tok(jet_ast* ast, size_t n);
static jet_token* jet_ast_consume_tok(jet_ast* ast);
static jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type);
static jet_token_type jet_ast_peekn_tok_type(jet_ast* ast, size_t n);
static bool jet_ast_is_tok_match(jet_ast* ast, jet_token_type tok_type);
static const char* jet_ast_get_type_name(jet_token_type tok_type);

// PARSING
static bool jet_ast_is_type_tok(jet_token_type tok_type);
static bool jet_ast_is_vdecl(jet_ast* ast);
static bool jet_ast_is_func_head(jet_ast* ast);

static jet_ast_node* jet_astn_parse_next_stmt(jet_ast* ast);
static jet_ast_node* jet_astn_parse_expr_stmt(jet_ast* ast);
static jet_ast_node* jet_astn_parse_expr(jet_ast* ast, size_t min_prec);
static jet_ast_node* jet_astn_parse_primary(jet_ast* ast);
static jet_ast_node* jet_astn_prog_parse(jet_ast* ast);
static jet_ast_node* jet_astn_block_parse(jet_ast* ast);
static jet_ast_node* jet_astn_ident_parse(jet_ast* ast);
static jet_ast_node* jet_astn_tdecl_parse(jet_ast* ast);
static jet_ast_node* jet_astn_vdecl_parse(jet_ast* ast);
static jet_ast_node* jet_astn_func_parse(jet_ast* ast);


jet_ast* jet_ast_create(jet_list* tok_list)
{    
    if(!tok_list)
    {
        fprintf(stderr, "error: cannot create ast, invalid token list.\n");
        return NULL;
    }

    jet_list* top_node_list = jet_list_create(8, sizeof(jet_ast_node));
    if(!top_node_list)
    {
        fprintf(stderr, "error: cannot create ast, could not create node list.\n");
        return NULL;
    }

    jet_ast* ast =  (jet_ast*)malloc(sizeof(jet_ast));
    if(!ast)
    {
        fprintf(stderr, "error: cannot create ast, failed to allocate memory.\n");
        return NULL;
    }

    ast->tok_list = tok_list;
    ast->top_node_list = top_node_list; 
    ast->prog_node = NULL;
    ast->tok_cursor = 0;

    printf("ast created successfully!\n");
    return ast;
}

bool jet_ast_dispose(jet_ast* ast)
{
    if(!ast)
    {
        fprintf(stderr, "error: cannot free, given jet_ast is NULL.\n");
        return false;
    }

    for(size_t i = 0; i < jet_list_count(ast->top_node_list); i++)
    {
        void* node = jet_list_get(ast->top_node_list, i);
        if(node)
            jet_ast_node_dispose((jet_ast_node*)node);
    }

    if(ast->top_node_list) jet_list_dispose(ast->top_node_list); 
    if(ast->prog_node) jet_ast_node_dispose(ast->prog_node);
    free(ast);
    printf("ast disposed!\n");
    return true;
}

bool jet_ast_generate_nodes(jet_ast* ast)
{
    if(ast == NULL)
    {
        fprintf(stderr, "error: cannot generate ast nodes, given ast is NULL.\n");
        return false;
    }

    if(ast->tok_list == NULL)
    {
        fprintf(stderr, "error: cannot generate ast nodes, ast was not initialized with valid token list.\n");
        return false;
    }
    
    if(ast->top_node_list == NULL)
    {
        fprintf(stderr, "error: cannot generate ast nodes, ast did not initialize top_node_list.\n");
        return false;
    }

    if(!jet_list_is_empty(ast->top_node_list))
        jet_list_clear(ast->top_node_list);
    
    ast->tok_cursor = 0;

    while(jet_ast_peekn_tok_type(ast, 0) != TOK_EOF)
    {
        if(jet_ast_peekn_tok_type(ast, 0) == TOK_INV)
        {
            fprintf(stderr, "error: cannot generate ast nodes, invalid token encountered.\n");
            return false;
        }

        jet_ast_node* node = jet_astn_parse_next_stmt(ast);
        if(!node)
        {
            fprintf(stderr, "error: cannot generate ast, unable to parse next stmt.\n");        
            return false;
        }
        jet_ast_push_node(ast, node);
    }

    size_t top_node_count = jet_list_count(ast->top_node_list);
    printf("* successfully generated %zu top level statements.\n", top_node_count);
    
    if(ast->prog_node)
        printf("* program entry (prog) node generated.\n");

    return true;
}

static void jet_ast_push_node(jet_ast* ast, jet_ast_node* node)
{
    if(node->node_type == AST_PROG)
    {
        if(ast->prog_node == NULL)
            ast->prog_node = node;
        else
        {
            fprintf(stderr, "error: cannot push node to ast, multiple program entry points.\n");
            return;
        }
    }
    else jet_list_append(ast->top_node_list, (const void*)node);
}

static jet_token* jet_ast_peek_tok(jet_ast* ast)
{
    return jet_ast_peekn_tok(ast, 0);
}

static jet_token* jet_ast_peekn_tok(jet_ast* ast, size_t n)
{ 
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    size_t count = jet_list_count(ast->tok_list);
    if(count <= 0)
    {
        fprintf(stderr, "wrn: ast->tok_list is empty.\n");        
        return NULL; 
    }
    else if(ast->tok_cursor + n >= count)
    {
        fprintf(stderr, "wrn: cannot peak ahead %zu tokens (cursor=%zu), index out of bounds.\n", n, ast->tok_cursor);
        return NULL;
    }
    return (jet_token*)jet_list_get(ast->tok_list, ast->tok_cursor + n);
}

static jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    if(ast->tok_cursor >= jet_list_count(ast->tok_list))
    {
        puts("end of token-list reached.");
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
    assert(ast->tok_list != NULL);
    if(ast->tok_cursor >= jet_list_count(ast->tok_list))
    {
        puts("- cannot consume, end of token-list reached.");
        return NULL;
    }
    ast->tok_cursor++;
    return (jet_token*)jet_list_get(ast->tok_list , ast->tok_cursor - 1);
}

static jet_token_type jet_ast_peekn_tok_type(jet_ast* ast, size_t n)
{
    jet_token* tok = jet_ast_peekn_tok(ast, n);
    if(tok == NULL)
        return TOK_EOF;
    return tok->type;
}

static bool jet_ast_is_tok_match(jet_ast* ast, jet_token_type tok_type)
{
    jet_token* tok = jet_ast_peek_tok(ast);
    if(!tok) return false;
    if(tok->type == tok_type)
        return true;
    return false;
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
    if(t != TOK_ASG || t != TOK_SEMI)
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

static jet_ast_node* jet_astn_parse_next_stmt(jet_ast* ast)
{
    if(!ast)
    {
        fprintf(stderr, "error: cannot parse next stmt, ast is null.\n");
        return NULL;
    }
    
    jet_ast_node* parsed_node = NULL;
    jet_token_type t = jet_ast_peekn_tok_type(ast, 0);
    
    if(t == TOK_EOF)
    {
        printf("* ast parsing complete, end of file reached.\n");
        return NULL;
    }
    else if(t == TOK_INV)
    {
        fprintf(stderr, "error: cannot parse next stmt, invalid token.\n");
        return NULL; 
    }    
    else if(t == TOK_KWD_PROG)
    {
        parsed_node = jet_astn_prog_parse(ast);
        if(!parsed_node)
        {
            fprintf(stderr, "error: unable to parse next stmt (prog)\n");
            return NULL;
        }
    }
    else if(jet_ast_is_vdecl(ast))
    {
        parsed_node = jet_astn_vdecl_parse(ast);
        if(!parsed_node)
        {
            fprintf(stderr, "error: cannot parse next stmt (vdecl)\n");
            return NULL;
        }
    }
    else if(jet_ast_is_func_head(ast))
    { 
        parsed_node = jet_astn_func_parse(ast);
        if(!parsed_node)
        {
            fprintf(stderr, "error: cannot parse next stmt (func)\n");
            return NULL;
        }
    }
    else 
    {
        parsed_node = jet_astn_parse_expr_stmt(ast);
        if(!parsed_node)
        {
            fprintf(stderr, "cannot parse next stmt, expected expression statement.\n");
            return NULL;
        }
    }

    if(parsed_node == NULL)
        fprintf(stderr, "error: unable to parse next stmt, no valid stmt sequences parsed.\n");

    return parsed_node;
}

static jet_ast_node* jet_astn_parse_expr_stmt(jet_ast* ast)
{
    jet_ast_node* expr = jet_astn_parse_expr(ast, 0);
    if(!expr)
    {
        fprintf(stderr, "error: cannot parse expr stmt, expected expr node.\n");
        return NULL;
    }
    jet_ast_expect_tok(ast, TOK_SEMI);
    return expr;
}

static jet_ast_node* jet_astn_prog_parse(jet_ast* ast)
{
    jet_ast_expect_tok(ast, TOK_KWD_PROG);
    jet_ast_node* block = jet_astn_block_parse(ast);
    if(block == NULL)
    {
        fprintf(stderr, "error: unable to parse prog block.\n");
        return NULL;
    }
    jet_ast_node* prog = jet_astn_prog_create(block);
    return prog;
}

static jet_ast_node* jet_astn_block_parse(jet_ast* ast)
{
    jet_ast_expect_tok(ast, TOK_LBRC);
    jet_list* stmt_list = jet_list_create(4, sizeof(jet_ast_node));
    if(!stmt_list)
    {
        fprintf(stderr, "error: cannot parse block, could not create node list.\n");
        return NULL;
    }
    while(jet_ast_peekn_tok_type(ast, 0) != TOK_RBRC)
    {
        jet_ast_node* stmt = jet_astn_parse_next_stmt(ast);
        if(stmt == NULL)
        {
            fprintf(stderr, "error: cannot parse block, unable to parse next stmt.\n");
            return NULL;
        }
        jet_list_append(stmt_list, (const void*)stmt);
    }
    jet_ast_expect_tok(ast, TOK_RBRC);
    jet_ast_node* block = jet_astn_block_create(stmt_list);
    return block;
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
    jet_list* param_list = jet_list_create(4, sizeof(jet_ast_node));
    if(!param_list)
    {
        fprintf(stderr, "error: cannot parse func, unable to create param list.\n");
        return NULL;
    }

    jet_ast_node* vdecl = NULL;
    jet_token_type t = TOK_EOF;
    while(jet_ast_peekn_tok_type(ast, 0) != TOK_RPAR)
    {
        vdecl = jet_astn_vdecl_parse(ast);
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
        jet_list_append(param_list, (const void*)vdecl);
    }
    jet_ast_expect_tok(ast, TOK_RPAR); 
    if(jet_list_is_empty(param_list))
    {
        jet_list_dispose(param_list);
        param_list = NULL;
    }

    jet_list* ret_type_list = jet_list_create(1, sizeof(jet_ast_node));
    jet_list_append(ret_type_list, (const void*)tdecl);
    
    jet_ast_node* func = jet_astn_fdecl_create(ident, ret_type_list, param_list);
    
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
        jet_list* arg_list = jet_list_create(4, sizeof(jet_ast_node));

        while(jet_ast_peekn_tok_type(ast, 0) != TOK_RPAR)
        {
            jet_ast_node* arg = jet_astn_parse_expr(ast, 0);
            if(!arg)
            {
                fprintf(stderr, "error: cannot parse primary expr, unable to parse call arg.\n");
                return NULL;
            }
            jet_list_append(arg_list, arg);

            if(jet_ast_peekn_tok_type(ast, 0) == TOK_COMMA)
                jet_ast_consume_tok(ast);
            else break;
        }
        jet_ast_expect_tok(ast, TOK_RPAR);
        if(jet_list_is_empty(arg_list))
        {
            jet_list_dispose(arg_list);
            arg_list = NULL;
        }
        out_node = jet_astn_call_create(out_node, arg_list);
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
    jet_ast_node_list_print(ast->top_node_list, 0);
    
    printf("=======================\n");
    jet_ast_node_print(ast->prog_node, 0);
    
    printf("PROG\n");
    printf("=======================\n\n");
}


















