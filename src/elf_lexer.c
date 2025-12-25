#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "elf_lexer.h"
#include "elf_defines.h"


t_elf_lexer* elf_create_lexer(const char* source)
{
    t_elf_lexer* lexer = (t_elf_lexer*)malloc(sizeof(t_elf_lexer));
    if(!lexer)
    {
        lexer = NULL;
        printf("__error: could not allocate lexer memory\n");
        return (t_elf_lexer*){0};
    }

    lexer->source = source;
    const size_t token_capacity = strlen(source);
    lexer->tokens = (t_elf_token**)malloc(sizeof(t_elf_token*) * token_capacity);
    if(!lexer->tokens)
    {
        lexer->tokens = NULL;
        printf("__error: could not allocate token-array memory\n");
        return (t_elf_lexer*){0};
    }

    printf("lexer created...\n");

    return lexer;
}

t_elf_token* elf_create_token(char c, t_token_type type)
{
    t_elf_token* token = (t_elf_token*)malloc(sizeof(t_elf_token));
    if(!token)
    {
        token = NULL;
        printf("__error: could not allocate token memory\n");
        return (t_elf_token*){0};
    }
    token->c = c;
    token->type = type;
    return token;
}


t_elf_token_type elf_get_tokentype(char c)
{
    switch(c)
    {
        default: 
        return INVALID_TOKEN;
        
        case 'a': case 'b': case 'c': case 'd' : case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y':
        case 'z': 
        case 'A': case 'B': case 'C': case 'D': case 'E':
        case 'G': case 'H': case 'I': case 'J': case 'K':
        case 'L': case 'M': case 'N': case 'O': case 'P':
        case 'Q': case 'R': case 'S': case 'T': case 'U':
        case 'U': case 'V': case 'W': case 'X': case 'Y':
        case 'Z': 
        case '_':
        return IDENTIFIER;
        
        case '0': case '1': case '2': case'3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        return DIGIT;

        case ' ':
        return WHITE_SPACE;

        case '.': 
        return DOT;

        case '+': case '-': case '*': case '/':
        case '%': case '!': case '^': case '=':
        return OPERATOR;

        case '"':
        return DB_QUOTE;

        case ';':
        return STMT_END;

        case '{': 
        return SCOPE_BEG;

        case '}':
        return SCOPE_END;

        case '(':
        return EXPR_BEG;

        case ')':
        return EXPR_END;

        case '\0':
        return TERMINATE;
    }
}






































