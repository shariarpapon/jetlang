#include <jet_error_print.h>
#include <jet_logger.h>

const char* jet_error_kind_str(jet_error_kind kind)
{
    switch(kind)
    {
        default: 
            return NULL;
            break;
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
    }

}

void jet_error_print_da(const jet_da* err_da)
{
    if(err_da == NULL) return;

    for(size_t i = 0; i < jet_da_count(err_da); i++)
    {
        const jet_error* e = (const jet_error*)jet_da_get(err_da, i);
        const char* kind_str = jet_error_kind_str(e->kind);
        if(kind_str == NULL) 
        {
            jet_log_outputf(JET_LOG_LEVEL_FATAL, "unrecognized_error_code: %d", (int)e->kind);
            continue;
        }
        jet_log_outputf(JET_LOG_LEVEL_ERROR, "[%zu:%zu]: %s", (size_t)e->span.line, (size_t)e->span.col, kind_str);
    }
 }
