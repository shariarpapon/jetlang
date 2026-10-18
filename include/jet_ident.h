#pragma once

typedef struct jet_ident
{
  const char* str;
  size_t len;
} jet_ident;

jet_ident jet_ident_init(const char* str, size_t len);
