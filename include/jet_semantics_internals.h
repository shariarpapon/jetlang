#pragma once
#include <jet_type.h>
#include <jet_ast_node.h>
#include <jet_compilation_unit.h>
#include <jet_hs.h>
#include <jet_ident.h>

#define JET_INVALID_TID (0)

// @construction
bool jet_sema_typedef_from_node(const jet_ast_node* node, jet_typedef* out);

// @types
size_t jet_sema_register_typedef(jet_compilation_unit* cu, jet_typedef def);
size_t jet_sema_tid_from_index(size_t index);
size_t jet_sema_index_from_tid(size_t tid);
bool jet_sema_init_type_id_hs(jet_hs* hs, size_t capacity);
bool jet_sema_add_type_id(jet_hs* hs, jet_ident type_name, size_t type_id);
bool jet_sema_has_type_name(const jet_hs* hs, jet_ident type_name);
size_t jet_sema_get_type_id(const jet_hs* hs, jet_ident type_name);
