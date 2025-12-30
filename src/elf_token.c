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


elf_TokenType elf_TokenType_eval(char c) { elf_TokenType type; switch(c) { default: type = TOK_invalid; break; case '\0': type = TOK_EOF; break; case ' ': case '\n': case '\t': case '\f': case '\r': case '\v': type = TOK_WSPACE; break; case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':               case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':                          case '_': type = TOK_IDENT; break; case '+': case '-': case '*': case '/': type = TOK_OPR; break; case '0': case '1' : case '2': case '3': case '4': case '5': case '6': case '7' : case '8': case '9': type = TOK_NUMINT; break; case '.': type = TOK_DOT; break;
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


