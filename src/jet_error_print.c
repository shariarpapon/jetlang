#include <string.h>
#include <stdio.h>

#include <jet_error_print.h>
#include <jet_logger.h>
#include <jet_sb.h>

#define JET_ERR_PRINT_BUF_SIZE (1024)

static bool jet_error_resolve_str(const jet_error* e, char* out_buf, size_t buf_size);

void jet_error_print(const jet_error* e)
{
    if(e == NULL) return;
    char buf[JET_ERR_PRINT_BUF_SIZE];
    JET_ASSERT(jet_error_resolve_str(e, buf, JET_ERR_PRINT_BUF_SIZE));
    jet_log_outputf(JET_LOG_LEVEL_ERROR, "[%zu:%zu]: %s", (size_t)e->span.line, (size_t)e->span.col, buf);
}

const char* jet_error_kind_str(jet_error_kind kind)
{
    switch(kind)
    {
        default: 
            return "## no_str_rep_for_err_kind ##";
        case JET_ERR_UNKNOWN:
             return "JET_ERR_UNKNOWN";
        case JET_ERR_UNEXPECTED_EOF:
             return "JET_ERR_UNEXPECTED_EOF";
        case JET_ERR_UNEXPECTED_TOKEN:
             return "JET_ERR_UNEXPECTED_TOKEN";
        case JET_ERR_EXPECTED_TOKEN: 
             return "JET_ERR_EXPECTED_TOKEN";
        case JET_ERR_INVALID_TOKEN:
             return "JET_ERR_INVALID_TOKEN";
        case JET_ERR_PARSING_FAILED:
             return "JET_ERR_PARSING_FAILED";
        case JET_ERR_EXPECTED_EXPR:
             return "JET_ERR_EXPECTED_EXPR";
        case JET_ERR_PRIMARY_EXPR_PARSING_FAILED:
             return "JET_ERR_PRIMARY_EXPR_PARSING_FAILED";
        case JET_ERR_NO_VALID_PARSING_SEQ:
             return "JET_ERR_NO_VALID_PARSING_SEQ";
        case JET_ERR_NODE_NOT_ALLOWED:
             return "JET_ERR_NODE_NOT_ALLOWED";
        case JET_ERR_TYPE_REDEF:
             return "JET_ERR_TYPE_REDEF";
    }
}

void jet_error_print_da(const char* title, const jet_da* err_da)
{
    if(err_da == NULL) return;
    size_t count = jet_da_count(err_da);
    
    printf("\n__%s [%zu]__\n", title, count);
    for(size_t i = 0; i < count; i++)
    {
        const jet_error* e = (const jet_error*)jet_da_get(err_da, i);
        jet_error_print(e);
    }
 }

static bool jet_error_resolve_str(const jet_error* e, char* out_buf, size_t buf_size)
{
    if(e == NULL || out_buf == NULL || buf_size == 0)
        return false;

    jet_sb sb;
    JET_ASSERT(jet_sb_init(&sb, 64));
    const char* kind_str = jet_error_kind_str(e->kind);
    switch(e->kind)
    {
        default:
            jet_sb_append_cstr(&sb, kind_str);
            break;
        case JET_ERR_PARSING_FAILED:
        {
            const char* node_type_str = jet_ast_node_type_str(e->as.parsing_failed.node_type);
            jet_sb_appendf(&sb, "failed to parse node %s", node_type_str);
            break;
        }
        case JET_ERR_EXPECTED_TOKEN:
        {
            jet_error_expected_token err = e->as.expected_token;
            const char* exp_str = jet_token_type_str(err.expected);
            const char* found_str = jet_token_type_str(err.found);
            jet_sb_appendf(&sb, "expected token %s but found %s", exp_str, found_str);
            break;
        }
        case JET_ERR_UNEXPECTED_TOKEN:
        {
            jet_error_unexpected_token err = e->as.unexpected_token;
            const char* unexp_str = jet_token_type_str(err.found);
            jet_sb_appendf(&sb, "unexpected token %s", unexp_str);
            break;
        }
        case JET_ERR_INVALID_TOKEN:
        {
            char* tok_str = NULL;
            jet_token_strdup(e->as.invalid_token.inv_tok, &tok_str);
            JET_ASSERT(tok_str != NULL);
            jet_sb_appendf(&sb, "invalid token encountered '%s'", tok_str);
            free((void*)tok_str);
            break;
        }
        case JET_ERR_NO_VALID_PARSING_SEQ:
        {
            jet_sb_append_cstr(&sb, "unable to find any valid token sequence to parse");
            break;
        }
        case  JET_ERR_NODE_NOT_ALLOWED:
        {
            jet_error_node_error node_err = e->as.node_error;
            const char* type_str = jet_ast_node_type_str(node_err.node_type);
            jet_sb_appendf(&sb, "'%s' is not allowed here.", type_str);
            break;
        }
        case JET_ERR_TYPE_REDEF:
        {  
            jet_ident type_name = e->as.type_redef.type_name;
            jet_sb_appendf(&sb, "redefinition of type <%.*s>", (int)type_name.len, type_name.str);
            break;
        }
    }
    bool success = jet_sb_copy(&sb, out_buf, buf_size);
    jet_sb_dispose(&sb);
    return success;
}













