#include <jet_ident.h>

jet_ident jet_ident_init(const char* str, size_t len)
{
    jet_ident ident = {.str = str, .len = len};
    return ident;
}

