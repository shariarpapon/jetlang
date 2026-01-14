#include <jet_ast.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static bool jet_ast_try_generate(jet_list* token_list, jet_ast* out_ast); 

jet_ast* jet_ast_create(jet_list* token_list)
{
    if(!token_list)
    {
        fprintf(stderr, "error: cannot create jet_ast, invalid arg token_list.\n");
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

    if(!jet_ast_try_generate(token_list, ast))
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
    free(ast);
    return true;
}

static bool jet_ast_try_generate(jet_list* token_list, jet_ast* out_ast)
{
    puts("parsing token list to ast...");
    if(!token_list)
    {
         fprintf(stderr, "error: cannot generate ast, arg token_list is invalid.\n");
         return false;
    }

    if(!out_ast)
    {
         fprintf(stderr, "error: cannot generate ast, arg out_ast is invalid.\n");
         return false;
    }
    puts("ast generated successfully!");
    return true;
}
