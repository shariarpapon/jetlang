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
#define JET_TYPE_PRINT_SUFFIX_SPACE (20)
#define JET_TYPE_PRINT_FMT ("%-zu    %-*s%s\n")

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
    const jet_type* type = NULL;
    char buf[JET_TYPE_PRINT_MAX_DETAIL_BUF_SIZE];
    for(size_t i = 0; i < count; i++)
    {
        type = (const jet_type*)jet_da_get(da, i);
        if(jet_type_resolve_str(type, buf, sizeof(buf)))
        {
            const char* kind_spec;
            switch(type->kind)
            {
                default:
                    kind_spec = "P";
                    break;
                case JET_TYPE_INVALID:
                    kind_spec = "I *";
                    break;
                case JET_TYPE_CUSTOM:
                    kind_spec = "U";
                    break;
            }
            jet_sb_appendf(&sb, JET_TYPE_PRINT_FMT, i, JET_TYPE_PRINT_SUFFIX_SPACE, buf, kind_spec);
        }
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
    jet_sb_appendf(&sb, "%-*.*s (%zu byt)", 10, (int)name.len, name.str, type->size_bytes);
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










