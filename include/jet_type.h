#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <jet_ident.h>

typedef enum jet_type_kind
{
    JET_TYPE_INVALID,
    JET_TYPE_CUSTOM,
    JET_TYPE_BOOL,
    JET_TYPE_INT,
    JET_TYPE_FLOAT,
    JET_TYPE_CHAR,
    JET_TYPE_STR,
} jet_type_kind;

typedef struct jet_type
{
    jet_ident type_name;
    jet_type_kind kind;
    size_t size_bytes;
} jet_type;

const char* jet_type_kind_str(jet_type_kind kind);
