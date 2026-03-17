#include <jet_token_print.h>
#include <jet_token.h>
#include <jet_da.h>
#include <jet_logger.h>

#include <stdio.h>
#include <inttypes.h>

#define TOK_TB_HEADER_FMT "%-7s %-17s %-10s %-10s %s\n\n"
#define TOK_TB_ENTRY_FMT "%-7zu %-17s %-10" PRIu32 " %-10" PRIu32 " %.*s\n"

void jet_token_tprint_da(const jet_da* tokens)
{
    if(!tokens)
    {
        JET_ERROR("error: cannot print tokens, token tokens null.\n");
        return;
    }
    size_t len = jet_da_count(tokens);
    if(len == 0)
    {
        printf("nothing to print, token darray is empty\n");
        return;
    }
    
    printf("\ntoken darray [%zu]:\n\n", len);
    printf(TOK_TB_HEADER_FMT, "index", "type", "line", "col", "value");        
   
    for(size_t i = 0; i < len; i++)
    {  
        jet_token* token = jet_da_get(tokens, i);
        const char* type_str = jet_token_type_str(token->type);
        size_t len = token->span.end - token->span.start;
        printf(TOK_TB_ENTRY_FMT, i, type_str, token->line, token->col, (int)len, token->lexeme);        
    }
    printf("\n");
}
