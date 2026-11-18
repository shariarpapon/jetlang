#include <stdio.h>
#include <string.h>

#include <jet_type_print.h>
#include <jet_type.h>
#include <jet_da.h>
#include <jet_sb.h>
#include <jet_logger.h>

#define JET_TYPE_PRINT_MAX_DETAIL_BUF_SIZE (128)
#define JET_TYPE_PRINT_DETAIL_SB_INIT_CAP (32)
#define JET_TYPE_PRINT_SB_INIT_CAP (256)
#define JET_TYPE_PRINT_FMT "%-9zu %-9zu %-.*s"
#define JET_TYPE_PRINT_HEADER_FMT "%-6s %-9s %-9s %-s"

#define JET_TYPE_PRINT_LINE_SEP "\n=====================================\n"

void jet_type_print_da(const jet_da* da)
{
    if(da == NULL || jet_da_count(da) == 0)
        return;

    size_t count = jet_da_count(da);
    if(da == 0)
        return;

    jet_sb sb;
    if(!jet_sb_init(&sb, JET_TYPE_PRINT_SB_INIT_CAP))
    {
        JET_LOG_ERROR("could not print type da, failed to init sb.");
        return;
    }
    printf(JET_TYPE_PRINT_LINE_SEP "registered types [%zu]\n", count);
    printf(JET_TYPE_PRINT_HEADER_FMT JET_TYPE_PRINT_LINE_SEP,"t-id", "#fields", "bytes", "name");
    const jet_type* type = NULL;
    char buf[JET_TYPE_PRINT_MAX_DETAIL_BUF_SIZE];
    for(size_t i = 0; i < count; i++)
    {
        type = (const jet_type*)jet_da_get(da, i);
        if(jet_type_resolve_str(type, buf, sizeof(buf)))
            jet_sb_appendf(&sb, "%-6zu %s\n", i+1, buf);
        else
            jet_sb_appendf(&sb, "%.*s _could not resolve detailed str rep_\n", (int)type->type_name.len, type->type_name.str);
    }
    printf("%s", jet_sb_view(&sb));
    jet_sb_dispose(&sb);
}

bool jet_type_resolve_str(const jet_type* type, char* buf, size_t buf_len)
{
    if(!type || !buf || buf_len == 0) 
        return false;

    jet_sb sb;
    if(!jet_sb_init(&sb, JET_TYPE_PRINT_DETAIL_SB_INIT_CAP))
    {
        JET_LOG_ERROR("could not resolve type str rep, failed to init sb.");
        return false;
    } 
    bool success = true;
    jet_ident name = type->type_name;
    size_t field_count = jet_da_count(&type->field_symbol_da);
    jet_sb_appendf(&sb, JET_TYPE_PRINT_FMT, field_count, type->size_bytes, (int)name.len, name.str);
    size_t full_len = jet_sb_len(&sb) + 1;
    if(full_len > buf_len)
    {
        success = false;
        goto cleanup;
    }
    memcpy(buf, jet_sb_view(&sb), full_len);

cleanup:
    jet_sb_dispose(&sb);
    return success;
}










