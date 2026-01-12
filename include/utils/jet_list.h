#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct jet_list jet_list;

jet_list* jet_list_create(size_t capacity, size_t elm_size);
bool jet_list_dispose(jet_list* v);
bool jet_list_append(jet_list* v, const void* data);
bool jet_list_remove(jet_list* v, size_t i);
bool jet_list_clear(jet_list* v);
void* jet_list_get(jet_list* v, size_t i);
size_t jet_list_count(jet_list* v);
































































