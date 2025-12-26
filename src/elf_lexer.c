#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <elf_lexer.h>

t_elf_lexer* elf_create_lexer(const char* source)
{
    t_elf_lexer* lexer = (t_elf_lexer*)malloc(sizeof(t_elf_lexer));
    if(!lexer)
    {
        lexer = NULL;
        fprintf(stderr, "error: could not allocate lexer memory\n");
        return (t_elf_lexer*){0};
    }

    const size_t initTokenCapacity = 128;
    lexer->source = source;
    lexer->tokens = tblist_create(initTokenCapacity);

    if(!lexer->tokens)
    {
        lexer->tokens = NULL;
        fprintf(stderr, "error: could not allocate token-array memory\n");
        return (t_elf_lexer*){0};
    }

    printf("lexer created...\n");

    return lexer;
}

t_elf_token* elf_create_token(const char* source, size_t start, size_t end)
{
    t_elf_token* token = (t_elf_token*)malloc(sizeof(t_elf_token));
    if(!token)
    {
        token = NULL;
        fprintf(stderr, "error: could not allocate token memory\n");
        return (t_elf_token*){0};
    }
    token->source = source;
    token->start = start;
    token->end = end;
    token->type = elf_get_tokentype(source, start, end);
    return token;
}


t_elf_token_type elf_get_tokentype(const char* source, size_t start, size_t end)
{
  return INVALID_TOKEN;  
}

void elf_lexer_generate_tokens(t_elf_lexer* lexer){}





































