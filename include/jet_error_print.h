#pragma once
#include <jet_error.h>
#include <jet_da.h>

void jet_error_print(const jet_error* e);
void jet_error_print_da(const char* title, const jet_da* err_da);
const char* jet_error_kind_str(jet_error_kind kind);
