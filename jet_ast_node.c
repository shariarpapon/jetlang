#include <jet_ast_node.h>
#include <stdlib.h>
#include <stdio.h>

//SHOULD PROBABLY ADD 'CREATE BASED ON TYPE' METHOD
//DISPOSE BASED ON TYPE
bool jet_ast_node_dispose(jet_ast_node* node)
{
    if(!node) 
    {
        fprintf(stderr, "error: cannot free, provided node is NULL.\n");
        return false;
    }

    switch(node->node_type)
    {
        default: free(node);
    }
    return true;
}
