#pragma once
#include <stddef.h>

#define NULL_TERM '\0'
#define BOOL int
#define TRUE 1
#define FALSE 0

char* load_text_file(const char* file_path, size_t* out_len);
void println_concated_n(const char* s, const char* c, size_t n);























