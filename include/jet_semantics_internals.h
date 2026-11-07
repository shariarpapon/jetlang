#pragma once
#include <jet_type.h>
#include <jet_ast_node.h>
#include <jet_compilation_unit.h>

// @Construction
bool jet_sema_typedef_from_node(const jet_ast_node* node, jet_typedef* out);


// @Utils
bool jet_sema_add_typedef(jet_compilation_unit* cu, jet_typedef def);
