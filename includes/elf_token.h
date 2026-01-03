#pragma once
#include <stdlib.h>

typedef enum TokenType
{
    TOK_INV=0,    // Invalid / unrecognized token type
    TOK_EOF,      // End of file

    //literals
    TOK_ID,   // identifiers
    TOK_KW,   // keywords (reserved words)
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
    TOK_PERCENT,    // %
    
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
    TOK_LBR,   // {
    TOK_RBR,   // }
    TOK_LBRK,  // [
    TOK_RBRK,  // ]

} elf_TokenType;


typedef struct Token 
{
    const char* source;
    size_t origin;
    size_t len;
    elf_TokenType type;
} elf_Token;


elf_Token* elf_Token_create(const char* source, size_t origin, size_t len, elf_TokenType type);
const char* elf_TokenType_str(elf_TokenType type);
void elf_Token_printList(elf_Token** list, size_t count);


