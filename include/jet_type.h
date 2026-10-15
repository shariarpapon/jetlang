#pragma once

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

const char* jet_type_kind_str(jet_type_kind kind);
