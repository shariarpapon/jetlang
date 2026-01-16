#include <jet_ast_node.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

jet_ast_node* jet_ast_node_create(jet_ast_node_type node_type)
{
    jet_ast_node* n = (jet_ast_node*)malloc(sizeof(jet_ast_node));
    assert(n != NULL);
    n->node_type = node_type;
    n->child_nodes = jet_list_create(2, sizeof(jet_ast_node));
    assert(n->child_nodes != NULL);
    return n;
}

bool jet_ast_node_dispose(jet_ast_node* node)
{
    if(!node) return false;  
    free(node);    
    return true;
}


