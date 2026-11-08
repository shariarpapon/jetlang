#pragma once
#include <stdbool.h>
#include <jet_compilation_unit.h>

bool jet_sema_init(jet_compilation_unit* cu);
bool jet_sema_resolve_types(jet_compilation_unit* cu);
bool jet_sema_resolve_symbols(jet_compilation_unit* cu);




