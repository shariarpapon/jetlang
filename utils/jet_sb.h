#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct jet_sb jet_sb;

jet_sb* jet_sb_create(size_t cap);
void jet_sb_dispose(jet_sb* sb);
void jet_sb_clear(jet_sb* sb);
size_t jet_sb_len(jet_sb* sb);

void jet_sb_append_char(jet_sb* sb, char c);
void jet_sb_append_cstr(jet_sb* sb, const char* s);
void jet_sb_append_u64(jet_sb* sb, uint64_t v);
void jet_sb_append_sizet(jet_sb* sb, size_t v);
void jet_sb_append_int(jet_sb* sb, int i);





