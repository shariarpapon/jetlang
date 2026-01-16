#include <jet_ast.h>
#include <jet_ast_node.h>
#include <jet_token.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//{{{ decl
static bool jet_ast_generate_tree(jet_ast* ast); 
// }}}

// {{{ hdr
jet_ast* jet_ast_create(jet_list* token_list)
{
    if(!token_list)
    {
        fprintf(stderr, "error: cannot create jet_ast, invalid arg token_list.\n");
        return NULL;
    }

    if(jet_list_count(token_list) == 0)
    {
        fprintf(stderr, "error: cannot create AST, token list empty.\n");
        return NULL;
    }

    jet_ast* ast = (jet_ast*)malloc(sizeof(jet_ast));
        
    if(!ast)
    {
        fprintf(stderr, "error: cannot create jet_ast, failed to alloc mmemory.\n");
        ast = NULL;
        return NULL;
    }
    
    memset(ast, 0, sizeof(jet_ast));
   
    ast->token_list = token_list;

    if(!jet_ast_generate_tree(ast))
    {
        free(ast);
        fprintf(stderr, "error: cannot create jet_ast, failed to generate AST.\n");
        return NULL;
    }

    return ast;
}

bool jet_ast_dispose(jet_ast* ast)
{ 
    if(!ast)
    {
        fprintf(stderr, "error: cannot dispose jet_ast, invalid arg ast.\n");
        return false;
    }
    jet_ast_node_dispose(ast->root_node);
    free(ast);
    return true;
}
// }}}

static bool jet_ast_generate_tree(jet_ast* ast)
{
    ast->root_node = jet_ast_node_create(AST_NODE_ROOT);
    if(!ast->root_node) return false;
    return true;
}








