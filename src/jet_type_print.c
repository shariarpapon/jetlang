#include <stdio.h>

#include <jet_type_print.h>
#include <jet_type.h>
#include <jet_da.h>

void jet_type_print_da(const jet_da* da)
{
    if(da == NULL)
        return;

    size_t count = jet_da_count(da);
    for(size_t i = 0; i < count; i++)
    {
        const jet_type* type = (const jet_type*)jet_da_get(da, i);
        const char* kind_str = jet_type_kind_str(type->kind);
        printf("%-zu  %-*s\n", i, 16, kind_str);
    }
}


