#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jet_token.h>
#include <jet_utils.h>

jet_token* jet_token_create(const char* source, size_t origin, size_t len, jet_token_type type)
{
    jet_token* token = (jet_token*)malloc(sizeof(jet_token));
    if(!token)
    {
        fprintf(stderr, "error: couldn't allocate token memory.");
        return (jet_token*){0};
    }

    token->source = source;
    token->origin = origin;
    token->len = len;
    token->type = type;
    return token;
}


void jet_token_print_array(jet_token** tok_array, size_t len)
{
    if(!tok_array)
    {
        fprintf(stderr, "error: cannot print tokens, token tok_array null.\n");
        return;
    }

    if(len == 0)
    {
        printf("# nothing to print, token vect is empty\n");
        return;
    }
    
    printf("\ntoken vector [%zu]:\n\n", len);
    
    printf("%-7s %-17s %-10s %s\n\n", "len", "type", "enum-id", "value");        
    for(size_t i = 0; i < len; i++)
    {  
        jet_token* token = *tok_array;
        tok_array++;
        const char* type_str = jet_token_type_str(token->type);
        printf("%-7zu %-17s %-10d %.*s\n",i+1, type_str,(int)token->type, (int)token->len, token->source + token->origin);        
    }
    printf("\n");
    printf("* generated %zu tokens\n\n", len);
}

const char* jet_token_type_str(jet_token_type type)
{
    switch(type)
    {
        default:            return "no_str_conv";
        case TOK_INV:       return "TOK_INV";
        case TOK_EOF:       return "TOK_EOF";
        
        case TOK_IDENT:     return "TOK_IDENT";
        case TOK_KWD_NULL:  return "TOK_KWD_NULL";
        case TOK_KWD_INT:   return "TOK_KWD_INT";
        case TOK_KWD_FLOAT: return "TOK_KWD_FLOAT";
        case TOK_KWD_STR:return "TOK_KWD_STR";
        case TOK_KWD_BYTE:  return "TOK_KWD_BYTE";
        case TOK_KWD_BOOL:  return "TOK_KWD_BOOL";
        case TOK_KWD_TRUE:  return "TOK_KWD_TRUE";
        case TOK_KWD_FALSE: return "TOK_KWD_FALSE";

        case TOK_KWD_IF:    return "TOK_KWD_IF";
        case TOK_KWD_ELSE:  return "TOK_KWD_ELSE";
        case TOK_KWD_FOR:   return "TOK_KWD_FOR";
        case TOK_KWD_WHILE: return "TOK_KWD_WHILE";
        
        case TOK_INT_LIT:   return "TOK_INT_LIT";
        case TOK_FLOAT_LIT: return "TOK_FLOAT_LIT";
        case TOK_STR_LIT:   return "TOK_STR_LIT";
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





























