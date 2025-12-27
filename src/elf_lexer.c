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

elf_TokenType elf_TokenType_eval(char c)
{
    elf_TokenType type;
    switch(c)
    {
        default:
        type = TOK_INVALID;
        break;
        
        case '\0':
        type = TOK_EOF;
        break;

        case ' ': case '\n': case '\t': 
        case '\f': case '\r': case '\v':
        type = TOK_WSPACE;
        break;

        case 'a': case 'b': case 'c': case 'd': case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y':
        case 'z':               
        case 'A': case 'B': case 'C': case 'D': case 'E':
        case 'F': case 'G': case 'H': case 'I': case 'J':
        case 'K': case 'L': case 'M': case 'N': case 'O':
        case 'P': case 'Q': case 'R': case 'S': case 'T':
        case 'U': case 'V': case 'W': case 'X': case 'Y':
        case 'Z':                          
        case '_': 
        type = TOK_IDENT;
        break;

        case '+': case '-': case '*': case '/':
        type = TOK_OPR;
        break;

        case '0': case '1' : case '2': case '3': case '4':
        case '5': case '6': case '7' : case '8': case '9':
        type = TOK_NUMINT;
        break;

        case '.': 
        type = TOK_DOT;
        break;
        
        case ';':
        type = TOK_SCOLON;
        break;

        case '>':
        type = TOK_GTHAN;
        break;

        case '<':
        type = TOK_LTHAN;
        break;

        case '=':
        type = TOK_EQL;
        break;

        case '{':
        type = TOK_OBRACE;
        break;

        case '}':
        type = TOK_CBRACE;
        break;

        case '(':
        type = TOK_OPARAN;
        break;

        case ')':
        type = TOK_CPARAN;
        break;

        case '|':
        type = TOK_BAR;
        break;

        case '#':
        type = TOK_POUND;
        break;

        case ',':
        type = TOK_COMMA;
        break;

        case '&':
        type = TOK_AMP;
        break;

        case '$':
        type = TOK_DOLLAR;
        break;

        case '^':
        type = TOK_CARET;
        break;

        case '"':
        type = TOK_DQUOTE;
        break;

        case '\'':
        type = TOK_SQUOTE;
        break;

        case '\\':
        type = TOK_BSLASH;
        break;
    }
    return type;

}

void elf_TokenType_trim(const char* src, size_t* i,  elf_TokenType trimType)
{
    char c = elf_TokenType_eval(src[*i]);
    elf_TokenType type = elf_TokenType_eval(c);
    if(type != trimType)
    {
        printf("warning: could not trim TokenType; origin TokenType does not match trim TokenType.");
        return;
    }

    while(type == trimType)
    {
        (*i)++;
        type = elf_TokenType_eval(src[*i]);
    }
}

void elf_Lexer_addToken(elf_Lexer* lexer, size_t origin, size_t len, elf_TokenType type)
{

}


void elf_Lexer_generateTokens(elf_Lexer* lexer)
{
    if(!lexer || !lexer->source || !lexer->tokens) 
    {
        fprintf(stderr, "error: unable to generate tokens; lexer not in valid state.\n");
        return;
    }

    size_t i = 0;
    size_t trueSize = (size_t)strlen(lexer->source) + 1;
   
    char c;  
    elf_TokenType type = TOK_INVALID;
    size_t origin = 0;    
    
    //TEST
    return;
    while(1)
    {        
        if(i >= trueSize)
        {
            fprintf(stderr, "error: cannot generate tokens; source char index out of bounds.\n");
            return;
        }
        
        c = lexer->source[i];
        type = elf_TokenType_eval(c);
        origin = i;
        switch(type)
        {
            case TOK_INVALID:
            fprintf(stderr, "syntax error: no valid token type\n");
            return;

            break;
            case TOK_WSPACE:
            case TOK_IDENT:
            case TOK_NUMINT:
            elf_TokenType_trim(lexer->source, &i, type);
            break;
            
            case TOK_EOF:
            printf("__end of source string reached__");
            break;

            case TOK_OBRACE:
            case TOK_CBRACE:
            case TOK_OPARAN:
            case TOK_CPARAN:
            case TOK_SCOLON:
            case TOK_BAR:
            case TOK_AMP:
            case TOK_COMMA:
            case TOK_DQUOTE:
            case TOK_FSLASH:
            case TOK_BSLASH:
            case TOK_LTHAN:
            case TOK_GTHAN:
            case TOK_EQL:
            i++;
            break;

            default:
            printf("warning: TokenType not supoprted\n");
            break;
        }
        
        //WORKING ON THIS
        size_t len = i - origin;
        elf_Lexer_addToken(lexer, origin, len, type);

        if(type == TOK_EOF)
            break;
        else
        {
            fprintf(stderr, "error: source string did not end with TOK_EOF.\n");
            break;
        }
   }

}

























