#pragma once
#include <jet_da.h>
#include <jet_token.h>
#include <jet_ast_node.h>

typedef struct jet_ast jet_ast;

jet_ast* jet_ast_create(const jet_da* tok_da);
bool jet_ast_dispose(jet_ast* ast);
bool jet_ast_generate_nodes(jet_ast* ast);

const jet_ast_node* jet_ast_node_get(const jet_ast* ast, node_id nid);
const jet_da* jet_ast_get_top_nid_da(const jet_ast* ast);
node_id jet_ast_get_prog_nid(const jet_ast* ast);
