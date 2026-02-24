#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct jet_str_builder jet_str_builder;

jet_str_builder* jet_sb_create(size_t capacity);
void jet_sb_dispose(jet_str_builder sb);
const char* jet_sb_view(jet_str_builder sb);

const char* jet_sb_push_str(jet_str_builder sb, const char* str);
const char* jet_sb_push_int(jet_str_builder sb, int i);
const char* jet_sb_push_sizet(jet_str_builder sb, size_t st);

const char* jet_sb_pushl(jet_str_builder sb);
const char* jet_sb_pushl_str(jet_str_builder sb, const char* str);
const char* jet_sb_pushl_int(jet_str_builder sb, int i);
const char* jet_sb_pushl_sizet(jet_str_builder sb, size_t z);







