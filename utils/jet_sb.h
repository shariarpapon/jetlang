#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct jet_sb jet_sb;

jet_sb* jet_sb_create(size_t cap);
void jet_sb_dispose(jet_sb* sb);
void jet_sb_clear(jet_sb* sb);
size_t jet_sb_len(jet_sb* sb);

void jet_sb_append_char(jet_sb* sb, char c);





