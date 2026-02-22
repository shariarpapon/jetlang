#pragma once
#include <jet_darray.h>

typedef enum
{
    TOK_INV=0,    // Invalid / unrecognized token type
    TOK_EOF,      // End of file

    TOK_IDENT,   // identifiers
        
    //native reserved identifiers / keywords
    TOK_KWD_PROG, // program entry point
    TOK_KWD_NEEDS,// dependency module specifier
    TOK_KWD_MEM,  // memory allocation keyword
    
    TOK_KWD_CHAR,   // type decl: char
    TOK_KWD_VOID,   // type decl: void
    TOK_KWD_INT,    // type decl: int
    TOK_KWD_FLOAT,  // type decl: loat
    TOK_KWD_STR,    // type decl: string
    TOK_KWD_BYTE,   // type decl: byte
    TOK_KWD_BOOL,   // type decl: boolean

    TOK_KWD_IF,     // decl: conditional if
    TOK_KWD_ELSE,   // decl: conditional-else
    TOK_KWD_FOR,    // decl: loop-for
    TOK_KWD_WHILE,  // decl: loop-while
    TOK_KWD_RETURN, // perform: return to last caller address
    TOK_KWD_BREAK,  // perform: break out of loops

    TOK_KWD_NULL, // placeholder asignment
    TOK_KWD_TRUE, // boolean value: true
    TOK_KWD_FALSE,// boolean value: false
    TOK_LIT_INT,   // 0..9 (numeric literal)
    TOK_LIT_FLOAT,  // decimal point numbers (e.g. 3.1415)
    TOK_LIT_STR,   // "string literal"
    TOK_LIT_CHAR,  // 'a'

    TOK_BCOM,      // /*block comment*/
    TOK_LCOM,      // //line comment

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
    
    TOK_SHL,       // <<
    TOK_SHR,       // >>

    //Arithmetic operators
    TOK_PLUS,       // + 
    TOK_MINUS,      // -
    TOK_STAR,       // *
    TOK_SLASH,      // /
    TOK_MOD,        // %
    
    TOK_PLUSEQ,     // +=
    TOK_MINEQ,      // -=
    TOK_MULEQ,      // *=
    TOK_DIVEQ,      // /=
    TOK_MODEQ,      // %=
    TOK_BANDEQ,    // &=
    TOK_BOREQ,     // |=
    TOK_XOREQ,     // ^=
    TOK_POW,        // **
    TOK_INCR,       // ++
    TOK_DECR,       // --

    //Delimiters
    TOK_COLON,  // :
    TOK_COMMA, // ,
    TOK_SEMI,  // ;
    TOK_DOT,   // . 
    TOK_LPAR,  // (
    TOK_RPAR,  // )
    TOK_LBRC,  // {
    TOK_RBRC,  // }
    TOK_LBRK,  // [
    TOK_RBRK,  // ]

} jet_token_type;


typedef struct 
{
    const char* source;
    size_t origin;
    size_t len;
    jet_token_type type;
} jet_token;


jet_token* jet_token_create(const char* source, size_t origin, size_t len, jet_token_type type);
const char* jet_token_type_str(jet_token_type type);
void jet_token_print_darray(jet_darray* v);
char* jet_token_strdup(jet_token* tok);


