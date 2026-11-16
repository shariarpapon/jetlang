#pragma once
#include <stdbool.h>
#include <jet_compilation_unit.h>

bool jet_sema_init(jet_compilation_unit* cu);
bool jet_sema_analyze(jet_compilation_unit* cu);

