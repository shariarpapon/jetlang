#pragma once
#include <jet_type.h>
#include <jet_ast_node.h>
#include <jet_compilation_unit.h>
#include <jet_hs.h>
#include <jet_ident.h>
#include <jet_error.h>

#define JET_INVALID_TID (0)
#define JET_INVALID_SCOPEID (0)
#define JET_INVALID_SYMID (0)

// types
size_t jet_sema_get_type_id(const jet_compilation_unit* cu, jet_ident type_name);
bool jet_sema_add_type_id(jet_compilation_unit* cu, jet_ident type_name, size_t type_id);
bool jet_sema_has_type_name(const jet_compilation_unit* cu, jet_ident type_name);
bool jet_sema_can_node_construct_type(const jet_ast_node* node);
bool jet_sema_type_from_node(const jet_ast_node* node, jet_type* out);
const jet_type* jet_sema_get_type_by_name(const jet_compilation_unit* cu, jet_ident type_name);
const jet_type* jet_sema_get_type_by_tid(const jet_compilation_unit* cu, size_t tid);


// semantic passes
void jet_sema_register_type(jet_compilation_unit* cu, jet_type def);
void jet_sema_register_primitive_types(jet_compilation_unit* cu);
void jet_sema_register_custom_types(jet_compilation_unit* cu);
void jet_sema_register_global_symbols(jet_compilation_unit* cu);

// id-index conversion
size_t jet_sema_index_to_tid(size_t index);
size_t jet_sema_tid_to_index(size_t tid);

size_t jet_sema_index_to_symid(size_t index);
size_t jet_sema_symid_to_index(size_t symid);

size_t jet_sema_index_to_scopeid(size_t idnex);
size_t jet_sema_scopeid_to_index(size_t scopeid);

// err
void jet_sema_push_err(jet_compilation_unit* cu, const jet_error* err);


























