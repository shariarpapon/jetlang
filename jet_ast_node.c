#include <jet_ast_node.h>
#include <stdlib.h>
#include <stdio.h>

bool jet_ast_node_dispose(jet_ast_node* node)
{
    if(!node) 
    {
        fprintf(stderr, "error: cannot free, provided node is NULL.\n");
        return false;
    }
    free(node);
    return true;
}
