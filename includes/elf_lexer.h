#pragma once
#include <toolbox/tblist.h>

typedef enum TokenType
{
    INVALID_TOKEN=0,
    IDENTIFIER,
    DIGIT,
    OPERATOR,
    WHITE_SPACE,
    DOT,
    DBL_QUOTE,
    SEMI_COLON,
    BRACE_OPEN,
    BRACE_CLOSE,
    PARAN_OPEN,
    PARAN_END,
    TERMINATE,
} t_elf_token_type;

typedef struct Token
{
    const char* source;
    size_t start;
    size_t end;
    t_elf_token_type type;
} t_elf_token;

typedef struct ElfLexer
{
    const char* source;
    tblist* tokens;
}t_elf_lexer;

t_elf_lexer* elf_create_lexer(const char* src);
t_elf_token* elf_create_token(const char* src, size_t start, size_t end);
t_elf_token_type elf_get_tokentype(const char* src, size_t start, size_t end);
void t_elf_lexer_generate_tokens(t_elf_lexer* lexer);


































