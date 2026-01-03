#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf_token.h>

elf_Token* elf_Token_create(const char* source, size_t origin, size_t len, elf_TokenType type)
{
    elf_Token* token = (elf_Token*)malloc(sizeof(elf_Token));
    if(!token)
    {
        fprintf(stderr, "error: couldn't allocate token memory.");
        return (elf_Token*){0};
    }

    token->source = source;
    token->origin = origin;
    token->len = len;
    token->type = type;
    return token;
}


void elf_Token_printList(elf_Token** tokens, size_t count)
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
        elf_Token* token = *tokens;
        tokens++;
        const char* typeStr = elf_TokenType_str(token->type);
        printf("%s  %.*s \n", typeStr, (int)token->len, token->source + token->origin);    
    }
}

const char* elf_TokenType_str(elf_TokenType type)
{
    return "null";
}




