#pragma once
#include <jet_ident.h>
#include <jet_type.h>
#include <jet_hs.h>

#define JET_INVALID_TID (0)

bool jet_tid_hs_init(jet_hs* hs, size_t capacity);
bool jet_tid_hs_add(jet_hs* hs, jet_ident type_name, size_t type_id);
bool jet_tid_hs_has_name(const jet_hs* hs, jet_ident type_name);
size_t jet_tid_hs_get_id(const jet_hs* hs, jet_ident type_name);













