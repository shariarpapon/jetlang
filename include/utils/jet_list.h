#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct jet_list jet_list;

jet_list* jet_list_create(size_t capacity, size_t elm_size);
bool jet_list_dispose(jet_list* list);
bool jet_list_clear(jet_list* list);

void* jet_list_bottom(jet_list* list);
void* jet_list_top(jet_list* list);

bool jet_list_insert(jet_list* list, size_t i, const void* data);
bool jet_list_append(jet_list* list, const void* data);
bool jet_list_prepend(jet_list* list, const void* data);

bool jet_list_shr_n(jet_list* list, size_t start, size_t n);
bool jet_list_shl_n(jet_list* list, size_t start, size_t n);
bool jet_list_shl(jet_list* list, size_t start);
bool jet_list_shr(jet_list* list, size_t start);
bool jet_list_remove(jet_list* list, size_t i);
bool jet_list_remove_top(jet_list* list);
bool jet_list_remove_bottom(jet_list* list);

bool jet_list_pinch(jet_list* list, size_t from, size_t qt, jet_list* out_list);

void* jet_list_get(jet_list* list, size_t i);
size_t jet_list_count(jet_list* list);
































































