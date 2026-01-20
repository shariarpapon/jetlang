#include <stdlib.h>
#include <stdio.h>
#include <jet_ast.h>
#include <jet_ast_node.h>

jet_ast* jet_ast_create(jet_list* tok_list)
{    
    if(!tok_list)
    {
        fprintf(stderr, "error: cannot create ast, invalid token list.\n");
        return NULL;
    }
    
    jet_list* node_list = jet_list_create(16, sizeof(jet_ast_node));
    if(!node_list)
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
    ast->node_list = node_list; 
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

    for(size_t i = 0; i < jet_list_count(ast->node_list); i++)
    {
        jet_ast_node* node = jet_list_get(ast->node_list, i);
        if(node) 
            jet_ast_node_dispose(node);
    }
    if(ast->node_list) jet_list_dispose(ast->node_list); 
    if(ast->prog_node) jet_ast_node_dispose(ast->prog_node);
    free(ast);
    return true;
}

