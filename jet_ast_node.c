#include <jet_ast_node.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

// NOTE: Returns a zero intialized node. 
// however, 
// the type union struct is not created since paramters are unknown.
jet_ast_node* jet_ast_node_create_base(jet_ast_node_type node_type)
{
    jet_ast_node* node = (jet_ast_node*)malloc(sizeof(jet_ast_node));
    if(!node)
    {
        fprintf(stderr, "error: cannot create node, failed to allocate memory.\n");
        return NULL;
    }
    node->node_type = node_type;
    return node;
}

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
