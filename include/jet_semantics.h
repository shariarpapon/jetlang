#pragma once
#include <stdbool.h>
#include <jet_compilation_unit.h>

bool jet_sema_init(jet_compilation_unit* cu);
bool jet_sema_resolve_types(jet_compilation_unit* cu);
bool jet_sema_resolve_symbols(jet_compilation_unit* cu);

const jet_type* jet_sema_get_type_by_name(const jet_compilation_unit* cu, jet_ident type_name);
const jet_type* jet_sema_get_type_by_tid(const jet_compilation_unit* cu, size_t tid);



