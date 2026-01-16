#pragma once
#include <stddef.h>

#define NULL_TERM '\0'
#define ARRAY_LEN(a) (sizeof(a) / sizeof(a[0]))

char* jet_io_read_text(const char* file_path, size_t* out_len);


















