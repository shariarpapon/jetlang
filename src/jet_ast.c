#include <jet_ast.h>
#include <stdlib.h>
#include <stdio.h>

static bool jet_ast_eval(jet_ast* ast);

jet_ast* jet_ast_create(jet_vector* token_vec)
{
    if(!token_vec)
    {
        perror("error: cannot create jet_ast with invalid token_vec.\n");
        return NULL;
    }

    jet_ast* ast = (jet_ast*)malloc(sizeof(jet_ast));
    if(!ast)
    {
        perror("error: cannot create jet_ast, failed to alloc mmemory.\n");
        ast = NULL;
        return NULL;
    }
    ast->token_vec = token_vec;
    return ast;
}

bool jet_ast_dispose(jet_ast* ast)
{ 
    if(!ast)
    {
        perror("error: cannot dispose jet_ast, invalid pointer.\n");
        return false;
    }
    free(ast);
    return true;
}

static bool jet_ast_eval(jet_ast* ast)
{
    if(!ast)
    {
        perror("error: cannot eval ast, invalid pointer.\n");
        return false;
    }
    return true;
}
