#include <stdio.h>

#include <jet_type_print.h>
#include <jet_type.h>
#include <jet_da.h>
#include <jet_sb.h>
#include <jet_logger.h>

#define JET_TYPE_PRINT_SB_INIT_CAP (256)
#define JET_TYPE_PRINT_COL_SPACE (20)
#define JET_TYPE_PRINT_FMT ("%-zu  %-*.*s\n")

void jet_type_print_da(const jet_da* da)
{
    if(da == NULL)
        return;

    jet_sb sb;
    JET_ASSERT(jet_sb_init(&sb, JET_TYPE_PRINT_SB_INIT_CAP));

    size_t count = jet_da_count(da);
    for(size_t i = 0; i < count; i++)
    {
        const jet_type* type = (const jet_type*)jet_da_get(da, i);
        jet_sb_appendf(&sb, JET_TYPE_PRINT_FMT, i, JET_TYPE_PRINT_COL_SPACE, (int)type->type_name.len, type->type_name.str);
    }
    printf("%s\n", jet_sb_view(&sb));
}


