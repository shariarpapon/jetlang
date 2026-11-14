#include <jet_symbol.h>

jet_symbol jet_symbol_init_base(jet_symbol_kind kind, jet_ident name, size_t type_id)
{
    return (jet_symbol)
    {
        .kind = kind,
        .name = name,
        .type_id = type_id
    };
}
