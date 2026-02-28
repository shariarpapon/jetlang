#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jet_token.h>
#include <jet_io.h>

#define TOK_TB_HEADER_FMT "%-7s %-17s %-10s %-10s %s\n\n"
#define TOK_TB_ENTRY_FMT "%-7zu %-17s %-10zu %-10zu %.*s\n"

void jet_token_print_darray(jet_da* tokens)
{
    if(!tokens)
    {
        fprintf(stderr, "error: cannot print tokens, token tokens null.\n");
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
        printf(TOK_TB_ENTRY_FMT, i, type_str, token->line, token->column, (int)token->len, token->source + token->origin);        
    }
    printf("\n");
    printf("* generated %zu tokens\n\n", len);
}

const char* jet_token_type_str(jet_token_type type)
{
    switch(type)
    {
        default:            return "unknown_str_conv";
        case TOK_INV:       return "TOK_INV";
        case TOK_EOF:       return "TOK_EOF";
        
        case TOK_IDENT:     return "TOK_IDENT";
        case TOK_KWD_MEM:   return "TOK_KWD_MEM";
        case TOK_KWD_CHAR:  return "TOK_KWD_CHAR";
        case TOK_KWD_NULL:  return "TOK_KWD_NULL";
        case TOK_KWD_INT:   return "TOK_KWD_INT";
        case TOK_KWD_FLOAT: return "TOK_KWD_FLOAT";
        case TOK_KWD_STR:   return "TOK_KWD_STR";
        case TOK_KWD_BYTE:  return "TOK_KWD_BYTE";
        case TOK_KWD_BOOL:  return "TOK_KWD_BOOL";
        case TOK_KWD_TRUE:  return "TOK_KWD_TRUE";
        case TOK_KWD_FALSE: return "TOK_KWD_FALSE";
        case TOK_KWD_PROG:  return "TOK_KWD_PROG";
        case TOK_KWD_NEEDS: return "TOK_KWD_NEEDS";
        case TOK_KWD_VOID:  return "TOK_KWD_VOID";

        case TOK_KWD_IF:    return "TOK_KWD_IF";
        case TOK_KWD_ELSE:  return "TOK_KWD_ELSE";
        case TOK_KWD_FOR:   return "TOK_KWD_FOR";
        case TOK_KWD_WHILE: return "TOK_KWD_WHILE";
        case TOK_KWD_BREAK: return "TOK_KWD_BREAK";
        case TOK_KWD_RETURN:return "TOK_KWD_RETURN";
        
        case TOK_LIT_INT:   return "TOK_LIT_INT";
        case TOK_LIT_FLOAT: return "TOK_LIT_FLOAT";
        case TOK_LIT_STR:   return "TOK_LIT_STR";
        case TOK_LIT_CHAR:  return "TOK_LIT_CHAR";
        case TOK_BCOM:      return "TOK_BCOM";
        case TOK_LCOM:      return "TOK_LCOM";
        
        case TOK_ASG:       return "TOK_ASG";
        case TOK_EQ:        return "TOK_EQ";
        case TOK_NEQ:       return "TOK_NEQ";
        case TOK_LT:        return "TOK_LT";
        case TOK_LTE:       return "TOK_LTE";
        case TOK_GT:        return "TOK_GT";
        case TOK_GTE:       return "TOK_GTE";
        case TOK_BAND:      return "TOK_BAND";
        case TOK_AND:       return "TOK_AND";
        case TOK_BOR:       return "TOK_BOR";
        case TOK_OR:        return "TOK_OR";
        case TOK_XOR:       return "TOK_XOR";
        case TOK_NOT:       return "TOK_NOT";
        case TOK_SHL:       return "TOK_SHL";
        case TOK_SHR:       return "TOK_SHR";
        case TOK_BANDEQ:    return "TOK_BANDEQ";
        case TOK_BOREQ:     return "TOK_BOREQ";
        case TOK_XOREQ:     return "TOK_XOREQ";

        case TOK_PLUS:      return "TOK_PLUS";
        case TOK_MINUS:     return "TOK_MINUS";
        case TOK_STAR:      return "TOK_STAR";
        case TOK_SLASH:     return "TOK_SLASH";
        case TOK_MOD:       return "TOK_MOD";
        
        case TOK_PLUSEQ:    return "TOK_PLUSEQ";
        case TOK_MINEQ:     return "TOK_MINEQ";
        case TOK_MULEQ:     return "TOK_MULEQ";
        case TOK_DIVEQ:     return "TOK_DIVEQ";
        case TOK_POW:       return "TOK_POW";
        case TOK_INCR:      return "TOK_INCR";
        case TOK_DECR:      return "TOK_DECR";

        case TOK_COLON:     return "TOK_COLON";
        case TOK_COMMA:     return "TOK_COMMA";
        case TOK_SEMI:      return "TOK_SEMI";
        case TOK_DOT:       return "TOK_DOT";
        case TOK_LPAR:      return "TOK_LPAR";
        case TOK_RPAR:      return "TOK_RPAR";
        case TOK_LBRC:      return "TOK_LBRC";
        case TOK_RBRC:      return "TOK_RBRC";
        case TOK_LBRK:      return "TOK_LBRK";
        case TOK_RBRK:      return "TOK_RBRK";
    }
}

char* jet_token_strdup(jet_token* tok)
{
    char* s = malloc(tok->len + 1);
    if(!s)
    {
        fprintf(stderr, "error: unable to allocate memory for str slice.\n");
        return NULL;
    }
    memcpy(s, tok->source + tok->origin, tok->len);
    s[tok->len] = '\0';
    return s;
}



























