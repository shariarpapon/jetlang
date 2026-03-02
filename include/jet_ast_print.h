#pragma once
#include <jet_ast_node.h>
#include <jet_da.h>
#include <stddef.h>

void jet_ast_print(jet_ast* ast);
void jet_ast_nid_da_print(jet_ast* ast, jet_da* nid_da, size_t depth);
void jet_ast_nid_print(jet_ast* ast, node_id nid, size_t depth);

