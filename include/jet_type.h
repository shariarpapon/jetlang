#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <jet_ident.h>

//NOTE: _name_str must be a string literal
#define JET_STATIC_CONSUTRCT_TYPE(_kind, _name_str, _size_bytes) \
    {.kind=(_kind), .size_bytes=(_size_bytes), .type_name={.str=(_name_str),.len=sizeof(_name_str)-1}}

typedef enum jet_type_kind
{
    JET_TYPE_INVALID,
    JET_TYPE_CUSTOM,
    JET_TYPE_VOID,
    JET_TYPE_BOOL,
    JET_TYPE_INT,
    JET_TYPE_FLOAT,
    JET_TYPE_CHAR,
    JET_TYPE_STR,
} jet_type_kind;

typedef struct jet_type
{
    jet_type_kind kind;
    jet_ident type_name;
    size_t size_bytes;
} jet_type; 

extern const jet_type jet_type_primitives[];
extern const size_t jet_type_primitive_count;
const char* jet_type_kind_str(jet_type_kind kind);
