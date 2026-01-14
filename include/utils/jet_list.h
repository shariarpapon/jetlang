#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct jet_list jet_list;

jet_list* jet_list_create(size_t capacity, size_t elm_size);

bool jet_list_dispose(jet_list* list);
bool jet_list_clear(jet_list* list);
bool jet_list_is_empty(jet_list* list);
bool jet_list_insert(jet_list* list, size_t i, const void* data);
bool jet_list_append(jet_list* list, const void* data);
bool jet_list_prepend(jet_list* list, const void* data);

bool jet_list_remove(jet_list* list, size_t i);
bool jet_list_remove_range(jet_list* list, size_t start, size_t end);
bool jet_list_remove_first(jet_list* list);
bool jet_list_remove_last(jet_list* list);

void* jet_list_get(jet_list* list, size_t i);
void* jet_list_first(jet_list* list);
void* jet_list_last(jet_list* list);
size_t jet_list_count(jet_list* list);
































































