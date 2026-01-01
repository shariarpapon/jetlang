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





