#include <jet_ast_node.h>

#include <string.h>
#include <stdio.h>

bool jet_ast_node_init(jet_ast_node* node, jet_ast_node_type type, size_t len, uint32_t line, uint32_t col)
{
    if(!node) return false;
    memset(node, 0, sizeof(*node));
    node->node_type = type;
    if(!jet_span_init(&node->span, len, line, col))
    {
        fprintf(stderr, "failed to init node, unable to init span.\n");
        return false;
    }
    return true;
}

bool jet_ast_node_dispose(jet_ast_node* node)
{
    //dispose according to type
    if(!node) 
        return false;
    return true;

}
