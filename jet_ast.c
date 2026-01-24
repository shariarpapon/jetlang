#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <jet_ast.h>
#include <jet_ast_node.h>

struct jet_ast 
{
   jet_list* tok_list;
   jet_list* top_node_list;
   jet_ast_node* prog_node;
   size_t tok_cursor;
};

static jet_ast_node* jet_ast_get_next_node(jet_ast* ast);
static void jet_ast_parse_primary(jet_ast* ast);
static void jet_ast_parse_expr(jet_ast* ast);

static jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type);
static jet_token* jet_ast_peek_tok(jet_ast* ast);
static jet_token* jet_ast_peek_next_tok(jet_ast* ast);
static jet_token* jet_ast_consume_tok(jet_ast* ast);

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
            jet_ast_node_dispose(node);
    }

    if(ast->top_node_list) jet_list_dispose(ast->top_node_list); 
    if(ast->prog_node) jet_ast_node_dispose(ast->prog_node);
    free(ast);
    return true;
}

static jet_ast_node* jet_ast_get_next_node(jet_ast* ast)
{
    assert(ast != NULL);
    return NULL;
}

static void jet_ast_parse_primary(jet_ast* ast)
{    
    assert(ast != NULL);
}

static void jet_ast_parse_expr(jet_ast* ast)
{
    assert(ast != NULL);
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
    assert(tok->type == tok_type);
    return jet_ast_consume_tok(ast);
}

static jet_token* jet_ast_peek_tok(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    if(jet_list_count(ast->tok_list) <= 0)
    {
        fprintf(stderr, "wrn: ast->tok_list is empty.\n");        
        return NULL; 
    }
    return (jet_token*)jet_list_get(ast->tok_list, ast->tok_cursor);
}

static jet_token* jet_ast_peek_next_tok(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    size_t tok_count = jet_list_count(ast->tok_list);
    if (tok_count <= 0)
    {
        fprintf(stderr, "wrn: ast->tok_list is empty.\n");        
        return NULL; 
    }
    if(ast->tok_cursor >= tok_count - 1)
    {
        puts("end of token-list reached.");
        return NULL;
    }
    return (jet_token*)jet_list_get(ast->tok_list, ast->tok_cursor + 1);
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





    
    




















