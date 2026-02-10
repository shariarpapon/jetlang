#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

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

static void jet_ast_push_node(jet_ast* ast, jet_ast_node* node);

static jet_ast_node* jet_ast_node_parse_expr(jet_ast* ast, size_t min_prec);
static jet_ast_node* jet_ast_node_parse_primary(jet_ast* ast);

static jet_token* jet_ast_peek_tok(jet_ast* ast);
static jet_token* jet_ast_peekn_tok(jet_ast* ast, size_t n);
static jet_token* jet_ast_consume_tok(jet_ast* ast);
static jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type);

jet_ast* jet_ast_create(jet_list* tok_list)
{    
    if(!tok_list)
    {
        fprintf(stderr, "error: cannot create ast, invalid token list.\n");
        return NULL;
    }

    jet_list* top_node_list = jet_list_create(2, sizeof(jet_ast_node));
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

static void jet_ast_push_node(jet_ast* ast, jet_ast_node* node)
{
    if(node->node_type == AST_PROG)
    {
        if(ast->prog_node == NULL)
            ast->prog_node = node;
        else
        {
            fprintf(stderr, "error: multiple program entry points.\n");
            return;
        }
    }
    else
    {
        jet_list_append(ast->top_node_list, (const void*)node);
    }
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
        fprintf(stderr, "wrn: cannot peak ahead %zu tokens (cursor=%zu), index out of bounds.", n, ast->tok_cursor);
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
        puts("end of token-list reached.");
        return NULL;
    }
    ast->tok_cursor++;
    return (jet_token*)jet_list_get(ast->tok_list , ast->tok_cursor - 1);
}


// PARSERS ******************************

static jet_ast_node* jet_ast_node_parse_expr(jet_ast* ast, size_t min_prec)
{
    jet_ast_node* lhs_node = jet_ast_node_parse_primary(ast);
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
        jet_ast_node* rhs_node = jet_ast_node_parse_expr(ast, op_prec + 1);        
        if(rhs_node == NULL)
        {
            fprintf(stderr, "wrn: cannot parse expression, expected rhs after operator.\n");
            return lhs_node;
        }
        lhs_node = jet_astn_binop_create(lhs_node, rhs_node, op_tok->type);
    }
    return lhs_node;
}

static jet_ast_node* jet_ast_node_parse_primary(jet_ast* ast)
{
    jet_token* cur_tok = jet_ast_peek_tok(ast);
    if(cur_tok == NULL) 
        return NULL;

    switch(cur_tok->type)
    {
        default:
            fprintf(stderr, "error: expected primary expression.\n");
            return NULL;
        case TOK_KWD_NULL:
        case TOK_KWD_TRUE:
        case TOK_KWD_FALSE:
        case TOK_LIT_INT:
        case TOK_LIT_FLOAT:
        case TOK_LIT_CHAR:
        case TOK_LIT_STR:
        {
            jet_ast_consume_tok(ast);
            jet_ast_node* lit = jet_astn_lit_create(cur_tok);
            return lit;
        }
        
        case TOK_IDENT:
        {
            jet_ast_consume_tok(ast);
            jet_ast_node* ident = jet_astn_ident_create(cur_tok->source + cur_tok->origin, cur_tok->len);
            return ident;
        }

        case TOK_LPAR:
        {
            jet_ast_consume_tok(ast);
            jet_ast_node* paran_expr = jet_ast_node_parse_expr(ast, 0);
            cur_tok = jet_ast_peek_tok(ast);
            if(cur_tok == NULL || cur_tok->type != TOK_RPAR)
            {
                fprintf(stderr, "error: expected ')' after primary expression.\n");
                return NULL;
            }
            jet_ast_consume_tok(ast);
            return paran_expr;
        }

        case TOK_NOT:
        case TOK_MINUS:
        {
            jet_ast_consume_tok(ast);
            jet_ast_node* rhs = jet_ast_node_parse_expr(ast, 0);
            if(rhs == NULL)
            {
                fprintf(stderr, "error: expected expr after '-'\n");
                return NULL;
            }
            jet_ast_node* unop = jet_astn_unop_create(rhs, cur_tok->type);
            return unop;
        }
    }  
    return NULL;
}

// DEBUG ******************************
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


















