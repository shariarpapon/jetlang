#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf_token.h>

elf_token* elf_token_create(const char* source, size_t origin, size_t len, elf_token_type type)
{
    elf_token* token = (elf_token*)malloc(sizeof(elf_token));
    if(!token)
    {
        fprintf(stderr, "error: couldn't allocate token memory.");
        return (elf_token*){0};
    }

    token->source = source;
    token->origin = origin;
    token->len = len;
    token->type = type;
    return token;
}


void elf_token_print_list(elf_token** tokens, size_t count)
{
    if(!tokens)
    {
        fprintf(stderr, "error: cannot print tokens, token list invalid.\n");
        return;
    }

    if(count == 0)
    {
        printf("# nothing to print, token list is empty\n");
        return;
    }
    
    for(size_t i = 0; i < count; i++)
    {  
        elf_token* token = *tokens;
        tokens++;
        const char* typeStr = elf_token_type_str(token->type);
        printf("%s  %.*s \n", typeStr, (int)token->len, token->source + token->origin);    
    }
}

const char* elf_token_type_str(elf_token_type type)
{
    switch(type)
    {
        default:          return "unknown";
        case TOK_INV:     return "TOK_INV";
        case TOK_EOF:     return "TOK_EOF";
        
        case TOK_IDENT:      return "TOK_IDENT";
        case TOK_KWD:      return "TOK_KWD"; 
        case TOK_NUM:     return "TOK_NUM";
        case TOK_STR:     return "TOK_STR";
        case TOK_BCOM:    return "TOK_BCOM";
        case TOK_LCOM:    return "TOK_LCOM";
        
        case TOK_ASG:     return "TOK_ASG";
        case TOK_EQ:      return "TOK_EQ";
        case TOK_NEQ:     return "TOK_NEQ";
        case TOK_LT:      return "TOK_LT";
        case TOK_LTE:     return "TOK_LTE";
        case TOK_GT:      return "TOK_GT";
        case TOK_GTE:     return "TOK_GTE";
        case TOK_BAND:    return "TOK_BAND";
        case TOK_AND:     return "TOK_AND";
        case TOK_BOR:     return "TOK_BOR";
        case TOK_OR:      return "TOK_OR";
        case TOK_XOR:     return "TOK_XOR";
        case TOK_NOT:     return "TOK_NOT";

        case TOK_PLUS:    return "TOK_PLUS";
        case TOK_MINUS:   return "TOK_MINUS";
        case TOK_STAR:    return "TOK_STAR";
        case TOK_SLASH:   return "TOK_SLASH";
        case TOK_MOD: return "TOK_MOD";
        
        case TOK_PLUSEQ:  return "TOK_PLUSEQ";
        case TOK_MINEQ:   return "TOK_MINEQ";
        case TOK_MULEQ:   return "TOK_MULEQ";
        case TOK_DIVEQ:   return "TOK_DIVEQ";
        case TOK_POW:     return "TOK_POW";

        case TOK_SEMI:    return "TOK_SEMI";
        case TOK_DOT:     return "TOK_DOT";
        case TOK_LPAR:    return "TOK_LPAR";
        case TOK_RPAR:    return "TOK_RPAR";
        case TOK_LBRC:     return "TOK_LBRC";
        case TOK_RBRC:     return "TOK_RBRC";
        case TOK_LBRCK:    return "TOK_LBRCK";
        case TOK_RBRCK:    return "TOK_RBRCK";
    }
}





























