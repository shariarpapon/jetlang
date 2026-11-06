#pragma once
#include <jet_ident.h>

typedef struct jet_symbol jet_symbol;
typedef enum jet_symbol_kind jet_symbol_kind;

enum jet_symbol_kind
{
    JET_SYMBOL_VAR,
    JET_SYMBOL_PARAM,
    JET_SYMBOL_FUNC,
    JET_SYMBOL_TYPE,
}; 

struct jet_symbol
{
    jet_symbol_kind kind;
    jet_ident name;
    size_t type_id;
};





















