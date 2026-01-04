#pragma once
#include <stdlib.h>

typedef enum
{
    TOK_INV=0,    // Invalid / unrecognized token type
    TOK_EOF,      // End of file

    //literals
    TOK_IDENT,   // identifiers
    TOK_KWD,   // keywords (reserved words)
    TOK_NUM,  // 0..9 (numeric literal)
    TOK_STR,  // "string literal"
    TOK_BCOM, // /*block comment*/
    TOK_LCOM, // //line comment

    //Comparison & logical operators
    TOK_ASG,       // = 
    TOK_EQ,        // ==  
    TOK_NEQ,       // !=
    TOK_LT,        // <
    TOK_LTE,       // <=
    TOK_GT,        // >
    TOK_GTE,       // >=
    TOK_BAND,      // &
    TOK_AND,       // &&
    TOK_BOR,       // |
    TOK_OR,        // ||
    TOK_XOR,       // ^
    TOK_NOT,       // !
    
    //Arithmetic operators
    TOK_PLUS,       // + 
    TOK_MINUS,      // -
    TOK_STAR,       // *
    TOK_SLASH,      // /
    TOK_MOD,    // %
    
    TOK_PLUSEQ,     // +=
    TOK_MINEQ,      // -=
    TOK_MULEQ,      // *=
    TOK_DIVEQ,      // /=
    TOK_MODEQ,      // %=
    TOK_POW,        // **

    //Punctuations
    TOK_SEMI,  // ;
    TOK_DOT,   // . 
    TOK_LPAR,  // (
    TOK_RPAR,  // )
    TOK_LBRC,   // {
    TOK_RBRC,   // }
    TOK_LBRK,  // [
    TOK_RBRK,  // ]

} elf_token_type;


typedef struct 
{
    const char* source;
    size_t origin;
    size_t len;
    elf_token_type type;
} elf_token;


elf_token* elf_token_create(const char* source, size_t origin, size_t len, elf_token_type type);
const char* elf_token_type_str(elf_token_type type);
void elf_token_print_list(elf_token** list, size_t count);


