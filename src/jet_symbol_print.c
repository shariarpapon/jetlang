#include <string.h>
#include <stdio.h>

#include <jet_symbol_print.h>
#include <jet_sb.h>
#include <jet_logger.h>
#include <jet_ident.h>

#define JET_SYM_PRINT_DA_SB_CAP (128)
#define JET_SYM_PRINT_SB_CAP (64)
#define JET_SYM_COL_BUF_LEN (256)

// col_space = 10
// sym_kind ___ sym_name ___ type_id
#define JET_SYM_PRINT_LINE_SEP "\n====================================\n"
#define JET_SYM_PRINT_HEADER_FMT "%-6s %-5s %-5s %s"
#define JET_SYM_PRINT_COL_FMT "%-6s %-5zu %-5zu %.*s"

static bool jet_symbol_print_resolve_str(const jet_symbol* sym, char buf[], size_t buf_len);

const char* jet_symbol_kind_str(jet_symbol_kind kind)
{
    switch(kind)
    {
        default:
            return "## sym_kind_no_str_rep ##";
        case JET_SYM_FUNC:
            return "FUNC";
        case JET_SYM_VAR:
            return "VAR";
    }
}

void  jet_symbol_print_da(const char* title, const jet_da* sym_da)
{
    if(!sym_da) return;

    jet_sb sb;
    JET_ASSERT(jet_sb_init(&sb, JET_SYM_PRINT_DA_SB_CAP));
    if(!title)
        title = "symbol da";
    size_t count = jet_da_count(sym_da);
    jet_sb_appendf(&sb, JET_SYM_PRINT_LINE_SEP "%s [%zu]\n", title, count);
    jet_sb_appendf(&sb,JET_SYM_PRINT_HEADER_FMT JET_SYM_PRINT_LINE_SEP, "kind", "t-id", "r-id", "name");

    char buf[JET_SYM_COL_BUF_LEN];
    for(size_t i = 0; i < count; i++)
    {
        const jet_symbol* sym = (const jet_symbol*)jet_da_get(sym_da, i);
        if(jet_symbol_print_resolve_str(sym, buf, sizeof(buf)))
            jet_sb_append_cstr(&sb, buf);
        else 
            jet_sb_append_cstr(&sb, "## could not resolve symbol str ##");
        jet_sb_append_cstr(&sb, "\n");
    }
    printf("%s", jet_sb_view(&sb));
    jet_sb_dispose(&sb);
}

static bool jet_symbol_print_resolve_str(const jet_symbol* sym, char buf[], size_t buf_len)
{
    if(!sym || !buf || buf_len == 0) 
        return false;

    jet_sb sb;
    JET_ASSERT(jet_sb_init(&sb, JET_SYM_PRINT_SB_CAP));

    const char* kind_str = jet_symbol_kind_str(sym->kind);
    jet_ident name = sym->name;

    JET_ASSERT(name.str || name.len == 0);
    jet_sb_appendf(&sb, JET_SYM_PRINT_COL_FMT, kind_str, sym->type_id, sym->record_id, (int)name.len, name.str);

    bool can_succeed = jet_sb_len(&sb) + 1 <= buf_len;

    if(!can_succeed)
        JET_LOG_ERROR("cannot resolve symbol str, output buffer too small.");
    else
        strcpy(buf, jet_sb_view(&sb));

    jet_sb_dispose(&sb);
    return can_succeed;
}





