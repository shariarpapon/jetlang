#pragma once

#include<stdlib.h>

typedef enum TokenType
{
    TOK_Invalid=0,
    TOK_EOF,

    //literals
    TOK_Identifier,
    TOK_Keyword,
    TOK_Number,
    TOK_String,
    TOK_BlockComment,
    TOK_LineComment,

    //Logics
    TOK_Assign,  // = 
    TOK_Equal,   // ==  
    TOK_NEqual,  // !=
    TOK_Less,    // <
    TOK_LEqual,  // <=
    TOK_Greater, // >
    TOK_GEqual,  // >=
    TOK_And,     // &&
    TOK_Or,      // ||
    TOK_Not,     // !
    
    //Mathematical operators
    TOK_Plus,   // + 
    TOK_Minus,  // -
    TOK_Star,   // *
    TOK_Slash,  // /

    //Punctuations
    TOK_Semicolon,  // ;
    TOK_Dot,        // . 
    TOK_LParen,     // (
    TOK_RParen,     // )
    TOK_LBrace,     // {
    TOK_RBrace,     // }
    TOK_LBracket,   // [
    TOK_RBracket,   // ]

} elf_TokenType;



typedef struct Token 
{
    const char* source;
    size_t origin;
    size_t len;
    elf_TokenType type;
} elf_Token;

elf_Token* elf_Token_create(const char* source, size_t origin, size_t len, elf_TokenType type);



