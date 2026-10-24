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
uint64_t jet_ident_hash(const void* ident_ptr);
bool jet_ident_equals(const void* ident_ptr_a, const void* ident_ptr_b);



