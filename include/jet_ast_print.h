#pragma once
#include <jet_ast.h>
#include <jet_ast_node.h>
#include <jet_da.h>
#include <stddef.h>

void jet_ast_print(const jet_ast* ast);
void jet_ast_nid_da_print(const jet_ast* ast, const jet_da* nid_da, size_t depth);
void jet_ast_nid_print(const jet_ast* ast, node_id nid, size_t depth);

