#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <elf_lexer.h>
#include <elf_token.h>
#include <elf_utils.h>
#include <toolbox/tbdefines.h>

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
    lexer->len = strlen(source) + 1;
    lexer->cursor = 0;

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

void elf_Lexer_emitToken(elf_Lexer* lexer, size_t origin, size_t len, elf_TokenType type)
{
    elf_Token* token = elf_Token_create(lexer->source, origin, len, type);
    if(tblist_append(lexer->tokens, token) == _false)
    {
        fprintf(stderr, "error: could not add new token to lexer->tokens.\n");
        return;
    }
}

_bool elf_Lexer_tokenize(elf_Lexer* lexer)
{
    if(!lexer || !lexer->tokens) 
    {
        fprintf(stderr, "error: cannot tokenize, lexer or lexer->tokens is invalid.\n");
        return _false;
    }
    char c;  
    while(1)
    {
        c = elf_Lexer_peek(lexer);
        if(elf_isWhiteSpace(c) == _true)
            elf_Lexer_trimNext(lexer); 

        if(c == '\0')
        {
            elf_Lexer_emitToken(lexer, lexer->cursor, 1, TOK_EOF);
            return _true;
        }
        
        if(elf_Lexer_consumeType(lexer, TOK_Identifier, elf_Lexer_isIdentifier))
            continue;
        if(elf_Lexer_consumeType(lexer, TOK_Number, elf_Lexer_isNumber))
            continue;
        if(elf_Lexer_consumeLineComment(lexer))
            continue;
        if(elf_Lexer_consumeBlockComment(lexer))
            continue;
    }

}

_bool elf_Lexer_isIdentifier(char c)
{
    switch (c)
    {
        case '_':
        case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        case 'g': case 'h': case 'i': case 'j': case 'k': case 'l':
        case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
        case 's': case 't': case 'u': case 'v': case 'w': case 'x':
        case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
        case 'G': case 'H': case 'I': case 'J': case 'K': case 'L':
        case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
        case 'S': case 'T': case 'U': case 'V': case 'W': case 'X':
        case 'Y': case 'Z': 
            return _true;
        default:
            return _false;
    }
    return _false;
}

_bool elf_Lexer_isNumber(char c)
{
    switch (c)
    {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return _true;
        default:
            return _false;
    }
    return _false;
}

char elf_Lexer_consume(elf_Lexer* lexer)
{
    char c = elf_Lexer_peek(lexer);
    elf_Lexer_next(lexer);
    return c;
}

_bool elf_Lexer_consumeLineComment(elf_Lexer* lexer)
{
    if(elf_Lexer_peek(lexer) != '/' || elf_Lexer_peekNext(lexer) != '/')
    {
        return _false;
    }

    size_t origin = lexer->cursor;
    elf_Lexer_consume(lexer);
    elf_Lexer_consume(lexer);

    while(elf_Lexer_peek(lexer) != '\0')
    {
        if(elf_Lexer_peek(lexer) == '\n' || 
           elf_Lexer_peek(lexer) == '\r')
        {
            elf_Lexer_consume(lexer);
            size_t len = lexer->cursor - origin;
            elf_Lexer_emitToken(lexer, origin, len, TOK_LineComment);
            return _true;        
        }
        elf_Lexer_consume(lexer);
    }
    
    return _false;
}

_bool elf_Lexer_consumeBlockComment(elf_Lexer* lexer)
{
    if(elf_Lexer_peek(lexer) != '/' || elf_Lexer_peekNext(lexer) != '*')
    {
        return _false;
    }

    size_t origin = lexer->cursor;
    elf_Lexer_consume(lexer);
    elf_Lexer_consume(lexer);
    while(elf_Lexer_peek(lexer) != '\0')
    {
        if(elf_Lexer_peek(lexer) == '*' && 
           elf_Lexer_peekNext(lexer) == '/')
        {
            elf_Lexer_consume(lexer);
            elf_Lexer_consume(lexer);
            size_t len = lexer->cursor - origin;
            elf_Lexer_emitToken(lexer, origin, len, TOK_BlockComment);
            return _true;
        }
        elf_Lexer_consume(lexer);
    }

    return _false;
}

_bool elf_Lexer_consumeType(elf_Lexer* lexer, elf_TokenType type, _bool(*isMatch)(char))
{
    if(!isMatch( elf_Lexer_peek(lexer)))
    {
        return _false;
    }

    size_t origin = lexer->cursor;
    elf_Lexer_consume(lexer);
    while(isMatch(elf_Lexer_peek(lexer)))
    {
        elf_Lexer_consume(lexer);
    }
    size_t len = lexer->cursor - origin;
    elf_Lexer_emitToken(lexer, origin, len, type);
    return _true;
}

char elf_Lexer_peekLast(elf_Lexer* lexer)
{
    size_t last = lexer->cursor - 1;
    if(last < 0)
    {
        printf("warning: currently cursor at first char, cannot peek last; will return EOF char.\n");
        return '\0';
    }
    return lexer->source[last];
}

char elf_Lexer_peekNext(elf_Lexer* lexer)
{
    size_t next = lexer->cursor + 1;
    if(next >= lexer->cursor)
    {
        printf("warning: currently cursor at last char, cannot peek next; will return EOF char.\n");
        return '\0';
    }
    return lexer->source[next];
}

char elf_Lexer_peek(elf_Lexer* lexer)
{
    return lexer->source[lexer->cursor];
}

void elf_Lexer_next(elf_Lexer* lexer)
{
    lexer->cursor++;
}

void elf_Lexer_trimNext(elf_Lexer* lexer)
{
    elf_Lexer_next(lexer);
    char c = elf_Lexer_peek(lexer);

    while(1)
    {
        if(elf_isWhiteSpace(c) == _true)
        { 
            elf_Lexer_next(lexer);
            c = elf_Lexer_peek(lexer);
        }
        else return;
    }
}








 












