#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <jet_ast.h>
#include <jet_ast_node.h>
#include <jet_ast_node_create.h>

struct jet_ast 
{
   jet_list* tok_list;
   jet_list* top_node_list;
   jet_ast_node* prog_node;
   size_t tok_cursor;
};

static void jet_ast_add_top_node(jet_ast* ast, jet_ast_node* node);
static bool jet_ast_generate(jet_ast* ast);
static jet_ast_node* jet_ast_get_next_node(jet_ast* ast);
static jet_ast_node* jet_ast_node_block_gen(jet_ast* ast);

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

static bool jet_ast_generate(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    if(jet_list_count(ast->tok_list) == 0)
    {
        fprintf(stderr, "erorr: cannot generate AST, token-list is empty.\n");
        return false;
    }

    jet_ast_node* cur_node = jet_ast_get_next_node(ast);
    while(cur_node  != NULL)
    {
        if(cur_node->node_type == AST_PROG)
        {
            if(ast->prog_node != NULL)
            {
                fprintf(stderr, "error: cannot generate AST, more than 1 program entry point found.\n");
                return false;
            }
            ast->prog_node = cur_node;
        }
        else jet_ast_add_top_node(ast, cur_node);        
        cur_node = jet_ast_get_next_node(ast);
    }
    return true;
}

static jet_ast_node* jet_ast_get_next_node(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    size_t tok_count = jet_list_count(ast->tok_list);
    if(tok_count == 0)
    {
        fprintf(stderr, "wrn: cannot eval node, token-list is empty.\n");
        return NULL;
    }
    if(ast->tok_cursor >= tok_count)
    {
        printf("end of token-list reached\n");
        return NULL;
    }

    jet_token* cur_tok = jet_ast_peek_tok(ast);
    if(cur_tok->type == TOK_EOF)
    {
        jet_ast_consume_tok(ast);
        return NULL;
    }

    jet_ast_node* node = NULL;

    switch(cur_tok->type)
    {
        default:
        case TOK_INV:
            fprintf(stderr, "error: could not evaluate valid node.");
            return NULL;
        case TOK_EOF:
            printf("EOF token reached.\n");
            return NULL;
        case TOK_KWD_PROG:
            node = jet_ast_node_create_base(AST_PROG);
            jet_ast_node* block = jet_ast_node_block_gen(ast);
            jet_ast_node_prog* prog = jet_astn_prog_create(block); 
            assert(block != NULL);
            assert(prog != NULL);
            node->as.prog = prog;
            break;
    }

    if(node == NULL)
    {
        fprintf(stderr, "error: could not generate AST node.\n");
    }
    return node;
}

static jet_ast_node* jet_ast_node_block_gen(jet_ast* ast)
{
    jet_list* node_list = jet_list_create(4, sizeof(jet_ast_node));
    assert(node_list != NULL);
    
    //TODO: populate node_list
    jet_ast_node_block* block = jet_astn_block_create(node_list);
    assert(block != NULL);
    jet_ast_node* node = jet_ast_node_create_base(AST_BLOCK);
    node->as.block = block;
    return node;
}

static void jet_ast_add_top_node(jet_ast* ast, jet_ast_node* node)
{
    assert(ast != NULL);
    assert(node != NULL);
    assert(ast->top_node_list != NULL);
    jet_list_append(ast->top_node_list, (const void*)node);
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





    
    




















