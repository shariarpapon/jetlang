#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <elf_lexer.h>
#include <elf_token.h>
#include <elf_utils.h>

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

void elf_Lexer_fullDispose(elf_Lexer* lexer)
{
    free(lexer->tokens);
    free(lexer);
}

void elf_Lexer_emitToken(elf_Lexer* lexer, size_t origin, size_t len, elf_TokenType type)
{
    elf_Token* token = elf_Token_create(lexer->source, origin, len, type);
    if(tblist_append(lexer->tokens, token) == FALSE)
    {
        fprintf(stderr, "error: could not add new token to lexer->tokens.\n");
        return;
    }
}

BOOL elf_Lexer_tokenize(elf_Lexer* lexer)
{
    if(!lexer || !lexer->tokens) 
    {
        fprintf(stderr, "error: cannot tokenize, lexer or lexer->tokens is invalid.\n");
        return FALSE;
    }
    char c;  
    while(1)
    {
        c = elf_Lexer_peek(lexer);
        if(elf_Lexer_isWhiteSpace(c) == TRUE)
            elf_Lexer_trimNext(lexer); 

        if(c == elf_EOF)
        {
            elf_Lexer_emitToken(lexer, lexer->cursor, 1, TOK_EOF);
            return TRUE;
        }
        
        if(elf_Lexer_consumeType(lexer, TOK_ID, elf_Lexer_isIdentifier))
            continue;
        if(elf_Lexer_consumeType(lexer, TOK_NUM, elf_Lexer_isNumber))
            continue;
        if(elf_Lexer_consumeLineComment(lexer))
            continue;
        if(elf_Lexer_consumeBlockComment(lexer))
            continue;
        if(elf_Lexer_consumeOperator(lexer))
            continue;
    }

}

BOOL elf_Lexer_isIdentifier(char c)
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
            return TRUE;
        default:
            return FALSE;
    }
    return FALSE;
}

BOOL elf_Lexer_isNumber(char c)
{
    switch (c)
    {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return TRUE;
        default:
            return FALSE;
    }
    return FALSE;
}

BOOL elf_Lexer_isWhiteSpace(char c)
{
    BOOL isWhiteSpace =  
            c == ' ' || 
            c == '\n' || 
            c == '\t' || 
            c == '\f' || 
            c == '\r' || 
            c == '\v';
    return isWhiteSpace;
}

BOOL elf_Lexer_isOperatorAtom(char c)
{
    switch(c)
    {
        case '=':
        case '>':
        case '<':
        case '|':
        case '&':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '^':
        case '!':
        return TRUE;
    }
    return FALSE;
}

elf_TokenType elf_Lexer_getAtomOprType(char c, BOOL* succ)
{
    *succ = FALSE;
    elf_TokenType oprType = TOK_INV;
    switch(c)
    {
        default:
            *succ = FALSE;
            oprType = TOK_INV; 
            break;
        case '=': 
            *succ = TRUE;
            oprType = TOK_ASG;
            break;
        case '>': 
            *succ = TRUE;
            oprType = TOK_GT;
            break;
        case '<': 
            *succ = TRUE;
            oprType = TOK_LT;
            break;
        case '!':
            *succ = TRUE;
            oprType = TOK_NOT;
            break;
        case '&':
            *succ =  TRUE;
            oprType = TOK_BAND;
            break;
        case '^':
            *succ = TRUE;
            oprType = TOK_XOR;
            break;
        case '|':
            *succ = TRUE;
            oprType = TOK_BOR;
            break;
        case '+':
            *succ = TRUE;
            oprType = TOK_PLUS;
            break;
        case '-':
            *succ = TRUE;
            oprType = TOK_MINUS;
            break;
        case '*':
            *succ = TRUE;
            oprType = TOK_STAR;
            break;
        case '/':
            *succ = TRUE;
            oprType = TOK_SLASH;
            break;
        case '%':
            *succ = TRUE;
            oprType = TOK_PERCENT;
            break;
    }
    return oprType;
}

elf_TokenType elf_Lexer_getCompOprType(elf_TokenType l, elf_TokenType r, BOOL* succ)
{
    *succ = FALSE;
    if(r == TOK_ASG)
    {
        if(l == TOK_ASG)
         {
            *succ = TRUE;
            return TOK_EQ;
         }
         else if(l == TOK_NOT)
         {
            *succ = TRUE;
            return TOK_NEQ;
         }
         else if(l == TOK_PLUS)
         {
            *succ = TRUE;
            return TOK_PLUSEQ;
         }
         else if(l == TOK_MINUS)
         {
            *succ = TRUE;
            return TOK_MINEQ;
         }
         else if(l == TOK_STAR)
         {
            *succ = TRUE;
            return TOK_MULEQ;
         }
         else if(l == TOK_SLASH)
         {
            *succ = TRUE;
            return TOK_DIVEQ;
         }
         else if(l == TOK_PERCENT)
         {
            *succ = TRUE;
            return TOK_MODEQ;
         }
         else if(l == TOK_GT)
         {
            *succ = TRUE;
            return TOK_GTE;
         }
         else if(l == TOK_LT)
         {
            *succ = TRUE;
            return TOK_LTE;
         }
    }
    else if(l == TOK_BOR && r == TOK_BOR)
    
    {
        *succ = TRUE;
        return TOK_OR;
    }
    else if(l == TOK_BAND && r == TOK_BAND)
    {
        *succ = TRUE;
        return TOK_AND;
    }
    else if(l == TOK_STAR && r == TOK_STAR)
    {
        *succ = TRUE;
        return TOK_POW;
    }
    
    *succ = FALSE;
    return TOK_INV;
}

char elf_Lexer_consume(elf_Lexer* lexer)
{
    char c = elf_Lexer_peek(lexer);
    elf_Lexer_next(lexer);
    return c;
}

BOOL elf_Lexer_consumeType(elf_Lexer* lexer, elf_TokenType type, BOOL(*isMatch)(char))
{
    if(!isMatch( elf_Lexer_peek(lexer)))
    {
        return FALSE;
    }

    size_t origin = lexer->cursor;
    elf_Lexer_consume(lexer);
    while(isMatch(elf_Lexer_peek(lexer)))
    {
        elf_Lexer_consume(lexer);
    }
    size_t len = lexer->cursor - origin;
    elf_Lexer_emitToken(lexer, origin, len, type);
    return TRUE;
}

BOOL elf_Lexer_consumeLineComment(elf_Lexer* lexer)
{
    if(elf_Lexer_peek(lexer) != '/' || elf_Lexer_peekNext(lexer) != '/')
    {
        return FALSE;
    }

    size_t origin = lexer->cursor;
    elf_Lexer_consume(lexer);
    elf_Lexer_consume(lexer);

    while(elf_Lexer_peek(lexer) != elf_EOF)
    {
        if(elf_Lexer_peek(lexer) == '\n' || 
           elf_Lexer_peek(lexer) == '\r')
        {
            elf_Lexer_consume(lexer);
            size_t len = lexer->cursor - origin;
            elf_Lexer_emitToken(lexer, origin, len, TOK_LCOM);
            return TRUE;        
        }
        elf_Lexer_consume(lexer);
    }
    
    return FALSE;
}

BOOL elf_Lexer_consumeBlockComment(elf_Lexer* lexer)
{
    if(elf_Lexer_peek(lexer) != '/' || elf_Lexer_peekNext(lexer) != '*')
    {
        return FALSE;
    }

    size_t origin = lexer->cursor;
    elf_Lexer_consume(lexer);
    elf_Lexer_consume(lexer);
    while(elf_Lexer_peek(lexer) != elf_EOF)
    {
        if(elf_Lexer_peek(lexer) == '*' && elf_Lexer_peekNext(lexer) == '/')
        {
            elf_Lexer_consume(lexer);
            elf_Lexer_consume(lexer);
            size_t len = lexer->cursor - origin;
            elf_Lexer_emitToken(lexer, origin, len, TOK_BCOM);
            return TRUE;
        }
        elf_Lexer_consume(lexer);
    }

    return FALSE;
}

BOOL elf_Lexer_consumeOperator(elf_Lexer* lexer)
{
    size_t len = 0;
    size_t origin = lexer->cursor;
    char curr = elf_Lexer_peek(lexer);
    char next = elf_Lexer_peekNext(lexer);

    BOOL* succ = elf_newBool();
    elf_TokenType oprType = elf_Lexer_getAtomOprType(curr, succ);
    if(*succ != TRUE)
    {
        elf_disposeBool(succ);
        return FALSE;
    }
 
    elf_Lexer_consume(lexer);
    len++;

    elf_TokenType nextOprType = elf_Lexer_getAtomOprType(next, succ);
    if(*succ == TRUE)
    { 
        elf_TokenType cmpdType = elf_Lexer_getCompOprType(oprType, nextOprType, succ);
        if(*succ == TRUE)
        {
            elf_Lexer_consume(lexer);
            len++;
            oprType = cmpdType;   
        }
    }   
    elf_Lexer_emitToken(lexer, origin, len, oprType);
    elf_disposeBool(succ);
    return len != 0;
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

char elf_Lexer_tryPeekAhead(elf_Lexer* lexer, size_t n, BOOL* succ)
{
    size_t i = lexer->cursor + n;
    if(i >= lexer->len)
    {
        *succ = FALSE; 
        return elf_EOF;
    }
    *succ = TRUE;
    return lexer->source[i];
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
        if(elf_Lexer_isWhiteSpace(c) == TRUE)
        { 
            elf_Lexer_next(lexer);
            c = elf_Lexer_peek(lexer);
        }
        else return;
    }
}








 












