#pragma once
#include <jet_da.h>
#include <jet_token.h>
#include <jet_ast_node.h>

typedef struct jet_ast 
{
    jet_da node_registry;
    jet_da top_nid_da;
    node_id prog_nid;
    size_t node_count;
} jet_ast;

bool jet_ast_init(jet_ast* ast, const jet_da* tok_da);
void jet_ast_dispose(jet_ast* ast);

node_id jet_ast_register_node(jet_ast* ast, const jet_ast_node* node);
bool jet_ast_push_nid(jet_ast* ast, node_id nid);

const jet_ast_node* jet_ast_node_get(const jet_ast* ast, node_id nid);
const jet_da* jet_ast_get_top_nid_da(const jet_ast* ast);
node_id jet_ast_get_prog_nid(const jet_ast* ast);




