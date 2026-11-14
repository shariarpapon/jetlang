#pragma once
#include <jet_hs.h>

typedef struct jet_ident
{
  const char* str;
  size_t len;
} jet_ident;

jet_ident jet_ident_init(const char* str, size_t len);
const jet_ident* jet_ident_hashset_add(jet_hs* hs, jet_ident ident);
bool jet_ident_hashset_init(jet_hs* hs, size_t capacity);
uint64_t jet_ident_hash(const jet_ident* ident_ptr);
bool jet_ident_value_equals(const jet_ident* a, const jet_ident* b);

bool jet_ident_id_kv_hs_init(jet_hs* hs, size_t cap);
bool jet_ident_id_kv_hs_add(jet_hs* hs, jet_ident key, size_t id);
bool jet_ident_id_kv_hs_has_key(const jet_hs* hs, jet_ident key, size_t* out_id);











