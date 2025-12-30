#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <elf_lexer.h>

elf_Lexer* elf_Lexer_create(const char* source)
{
    elf_Lexer* lexer = (elf_Lexer*)malloc(sizeof(elf_Lexer));
    if(!lexer)
    {
        lexer = NULL;
        fprintf(stderr, "error: could not allocate lexer memory\n");
        return (elf_Lexer*){0};
    }

    lexer->source = source;
    
    const size_t initTokenCapacity = 128;
    lexer->tokens = tblist_create(initTokenCapacity);

    if(!lexer->tokens)
    {
        lexer->tokens = NULL;
        fprintf(stderr, "error: could not allocate token-array memory\n");
        return (elf_Lexer*){0};
    }

    printf("lexer created...\n");

    return lexer;
}

























