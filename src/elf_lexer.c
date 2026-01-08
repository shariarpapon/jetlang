#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <elf_lexer.h>
#include <elf_token.h>
#include <elf_utils.h>

#define KWD_COUNT (sizeof(kwd_table) / sizeof(kwd_table[0]))
#define PUNCT_COUNT (sizeof(punct_table) / sizeof(punct_table[0]))
#define CMPD_PUNCT_COUNT (sizeof(cmpd_punct_table) / sizeof(cmpd_punct_table[0]))

typedef struct 
{
    const char* tok_value;
    elf_token_type tok_type;
} kwd_token_def;

typedef struct
{
    const char tok_value;
    elf_token_type tok_type;
} punct_token_def;

typedef struct
{
    const char tok_value_left;
    const char tok_value_right;
    elf_token_type tok_type;
} cmpd_punct_token_def;

static const punct_token_def punct_table[] = 
{
    {'=', TOK_ASG   },    
    {'&', TOK_BAND  },
    {'|', TOK_BOR   },
    {'^', TOK_XOR   },
    {'!', TOK_NOT   },
    {'%', TOK_MOD   },
    {'<', TOK_LT    },  {'>', TOK_GT    },      
    {'+', TOK_PLUS  },  {'-', TOK_MINUS }, 
    {'*', TOK_STAR  },  {'/', TOK_SLASH },  
    {':', TOK_COLON },  {',', TOK_COMMA }, 
    {';', TOK_SEMI  },  {'.', TOK_DOT   },
    {'(', TOK_LPAR  },  {')', TOK_RPAR  },
    {'{', TOK_LBRC  },  {'}', TOK_RBRC  },
    {'[', TOK_LBRK  },  {']', TOK_RBRK  },
};

static const cmpd_punct_token_def cmpd_punct_table[] = 
{
    {'=', '=', TOK_EQ     }, {'!', '=', TOK_NEQ   },
    {'<', '=', TOK_LTE    }, {'>', '=', TOK_GTE   },
    {'&', '&', TOK_AND    }, {'|', '|', TOK_OR    },
    {'+', '=', TOK_PLUSEQ }, {'-', '=', TOK_MINEQ },
    {'*', '=', TOK_MULEQ  }, {'/', '=', TOK_DIVEQ },
    {'%', '=', TOK_MODEQ  }, {'*', '*', TOK_POW   },
};

static const kwd_token_def kwd_table[] = 
{ 
   { "null",   TOK_KWD_NULL    }, 
   { "bool",   TOK_KWD_BOOL    },  
   { "true",   TOK_KWD_TRUE    },
   { "false",  TOK_KWD_FALSE   },
   { "int",    TOK_KWD_INT     },   
   { "float",  TOK_KWD_FLOAT   }, 
   { "str",    TOK_KWD_STRING  },  
   { "byte",   TOK_KWD_BYTE    }, 
   { "if",     TOK_KWD_IF      }, 
   { "else",   TOK_KWD_ELSE    }, 
   { "for",    TOK_KWD_FOR     }, 
   { "while",  TOK_KWD_WHILE   }, 
};

elf_lexer* elf_lexer_create(const char* source)
{
    printf("creating lexer...");

    elf_lexer* lexer = (elf_lexer*)malloc(sizeof(elf_lexer));
    if(!lexer)
    {
        lexer = NULL;
        fprintf(stderr, "error: could not allocate lexer memory\n");
        return (elf_lexer*){0};
    }

    lexer->source = source;
    lexer->len = strlen(source) + 1;
    lexer->cursor = 0;
    lexer->emit_comments = false;

    const size_t tok_capacity = 64;
    lexer->tokens = vec_create(tok_capacity);

    if(!lexer->tokens)
    {
        lexer->tokens = NULL;
        fprintf(stderr, "error: could not allocate token-array memory\n");
        return (elf_lexer*){0};
    }

    printf("successful!\n");
    return lexer;
}

void elf_lexer_full_dispose(elf_lexer* lexer)
{
    printf("full disposing lexer...");
    for(int i = 0; i < lexer->tokens->count; i++)
    {
        elf_token* tok = vec_get(lexer->tokens, i);
        if(tok) free(tok);
    }
    vec_dispose(lexer->tokens);
    free(lexer);
    printf("successful!\n");
}

void elf_lexer_emit_token(elf_lexer* lexer, size_t origin, size_t len, elf_token_type type)
{
    static size_t call_count = 0;
    call_count++;
    elf_token* token = elf_token_create(lexer->source, origin, len, type); 
    if(vec_append(lexer->tokens, token) == false)
    {
        fprintf(stderr, "error: could not add new token to lexer->tokens.\n");
        return;
    }
}

bool elf_lexer_tokenize(elf_lexer* lexer)
{
    printf("tokenizing...\n");
    if(!lexer || !lexer->tokens) 
    {
        fprintf(stderr, "error: cannot tokenize, lexer or lexer->tokens is invalid.\n");
        return false;
    }

    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
            if (elf_lexer_try_scan_whitespace(lexer))       continue;         
            else if (elf_lexer_try_scan_line_com(lexer))    continue;    
            else if (elf_lexer_try_scan_block_com(lexer))   continue;     
            else if (elf_lexer_try_scan_ident(lexer))       continue;   
            else if (elf_lexer_try_scan_char(lexer))        continue;   
            else if (elf_lexer_try_scan_num(lexer))         continue;
        
        elf_lexer_emit_token(lexer, lexer->cursor, 1, TOK_INV);
        elf_lexer_consume(lexer);
    } 
    printf("tokenization complete!\n");
    return true;
}

bool elf_lexer_is_ident(char c)
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
            return true;
        default:
            return false;
    }
    return false;
}

bool elf_lexer_is_digit(char c)
{
    switch (c)
    {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return true;
        default:
            return false;
    }
    return false;
}

bool elf_lexer_is_whitespace(char c)
{
    bool is_whitespace =  
            c == ' ' || 
            c == '\n' || 
            c == '\t' || 
            c == '\f' || 
            c == '\r' || 
            c == '\v';
    return is_whitespace;
}

bool elf_lexer_try_get_kwd_type(const char* s, size_t len, elf_token_type* out_tok_type)
{ 
    if(!out_tok_type)
    {
        perror("invalid output pointer");
        return false;
    }

    char token_str[len + 1];
    memcpy(token_str, (void*)s, len);
    token_str[len] = NULL_TERM;
    for(size_t i = 0; i < KWD_COUNT; i++)
    {
       if(strcmp(token_str, kwd_table[i].tok_value) == 0)
       {
            *out_tok_type = kwd_table[i].tok_type;
            return true;
       } 
    }

    return false;
}

bool elf_lexer_try_get_punct_type(char c, elf_token_type* out_tok_type) 
{
    if(!out_tok_type)
    {
        perror("invalid output pointer");
        return false;
    }
    
    for(size_t i = 0; i < PUNCT_COUNT; i++)
    {
        if(punct_table[i].tok_value == c)
        {
            *out_tok_type = punct_table[i].tok_type;
            return true;
        }
    }

    return false; 
}

bool elf_lexer_try_get_cmpd_punct_type(char left, char right, elf_token_type* out_tok_type) 
{
    if(!out_tok_type)
    {
        fprintf(stderr, "error: invalid output pointer\n");
        return false;
    }

    if (right == '=') 
    {
        if       (left == '=')  {  *out_tok_type = TOK_EQ;     }
        else if  (left == '!')  {  *out_tok_type = TOK_NEQ;    }
        else if  (left == '+')  {  *out_tok_type = TOK_PLUSEQ; }
        else if  (left == '-')  {  *out_tok_type = TOK_MINEQ;  }
        else if  (left == '*')  {  *out_tok_type = TOK_MULEQ;  }
        else if  (left == '/')  {  *out_tok_type = TOK_DIVEQ;  }
        else if  (left == '%')  {  *out_tok_type = TOK_MODEQ;  }
        else if  (left == '>')  {  *out_tok_type = TOK_GTE;    }
        else if  (left == '<')  {  *out_tok_type = TOK_LTE;    }
    }
    else if  (left == '|' && right == '|') { *out_tok_type = TOK_OR; }
    else if  (left == '&' && right == '&') { *out_tok_type = TOK_AND; }
    else if  (left == '*' && right == '*') { *out_tok_type = TOK_POW; }  
    else return false;
    return true;
}

bool elf_lexer_try_scan_ident(elf_lexer* lexer)
{
    if(elf_lexer_is_ident(elf_lexer_peek(lexer)) == false)
    {
        return false;
    }

    size_t origin = lexer->cursor;
    elf_lexer_consume(lexer);

    while(elf_lexer_is_ident(elf_lexer_peek(lexer))
       || elf_lexer_is_digit(elf_lexer_peek(lexer)) )
    {
        elf_lexer_consume(lexer);
    }
    elf_token_type tok_type = TOK_IDENT;
    elf_lexer_try_get_kwd_type(lexer->source + origin, lexer->cursor - origin, &tok_type);
    elf_lexer_emit_token(lexer, origin, lexer->cursor - origin, tok_type); 
    return true;
}

bool elf_lexer_try_scan_char(elf_lexer* lexer)
{
    size_t origin = lexer->cursor;
    char curr = elf_lexer_peek(lexer);
    char next = elf_lexer_peek_next(lexer);

    elf_token_type emit_tok_type = TOK_INV;
    if(!elf_lexer_try_get_punct_type(curr, &emit_tok_type))
    {
        return false;
    }

    elf_lexer_consume(lexer);

    if(elf_lexer_try_get_cmpd_punct_type(curr, next, &emit_tok_type))
    {
        elf_lexer_consume(lexer);
    }

    elf_lexer_emit_token(lexer, origin, lexer->cursor - origin, emit_tok_type);
    return true;
}

bool elf_lexer_try_scan_num(elf_lexer* lexer)
{
    if(elf_lexer_is_digit(elf_lexer_peek(lexer)) == false)
    {
        return false;
    }

    size_t origin = lexer->cursor;
    elf_lexer_consume(lexer); 
    while(elf_lexer_is_digit(elf_lexer_peek(lexer)))
    {
        elf_lexer_consume(lexer);
    }
    size_t len = lexer->cursor - origin;
    elf_lexer_emit_token(lexer, origin, len, TOK_NUM_LIT);
    return len != 0;
}


bool elf_lexer_try_scan_line_com(elf_lexer* lexer)
{
    if(elf_lexer_peek(lexer) != '/' || elf_lexer_peek_next(lexer) != '/')
    {
        return false;
    }

    size_t origin = lexer->cursor;
    elf_lexer_consume(lexer);
    elf_lexer_consume(lexer);

    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        if(elf_lexer_peek(lexer) == '\n')
        {
            size_t len = lexer->cursor - origin;
            elf_lexer_consume(lexer);
            if(lexer->emit_comments == true)
                elf_lexer_emit_token(lexer, origin, len, TOK_LCOM);
            return true;        
        }
        elf_lexer_consume(lexer);
    }   
    return false;
}

bool elf_lexer_try_scan_block_com(elf_lexer* lexer)
{
    if(elf_lexer_peek(lexer) != '/' || elf_lexer_peek_next(lexer) != '*')
    {
        return false;
    }

    size_t origin = lexer->cursor;
    elf_lexer_consume(lexer);
    elf_lexer_consume(lexer);
    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        if(elf_lexer_peek(lexer) == '*' && elf_lexer_peek_next(lexer) == '/')
        {
            elf_lexer_consume(lexer);
            elf_lexer_consume(lexer);
            size_t len = lexer->cursor - origin; 
            if(lexer->emit_comments == true)
                elf_lexer_emit_token(lexer, origin, len, TOK_BCOM);
            return true;
        }
        elf_lexer_consume(lexer);
    }

    return false;
}

bool elf_lexer_try_scan_whitespace(elf_lexer* lexer)
{
    if(elf_lexer_is_whitespace(elf_lexer_peek(lexer)) == true)
    {
        elf_lexer_consume(lexer);
        while(elf_lexer_is_whitespace(elf_lexer_peek(lexer)) == true)
            elf_lexer_consume(lexer);
        return true;
    }
    return false;
}

char elf_lexer_peek_prev(elf_lexer* lexer)
{
    size_t prev = lexer->cursor - 1;
    if(prev < 0)
    {
        printf("wrn: cannot peek prev while at cursor = 0\n");
        return NULL_TERM;
    }
    return lexer->source[prev];
}

char elf_lexer_peek_next(elf_lexer* lexer)
{
    size_t next = lexer->cursor + 1;
    if(next >= lexer->len - 1)
    {
        printf("wrn: cannot peek next, eof reached\n");
        return NULL_TERM;
    }
    return lexer->source[next];
}

char elf_lexer_peek(elf_lexer* lexer)
{
    return lexer->source[lexer->cursor];
}

char elf_lexer_consume(elf_lexer* lexer)
{
    char c = elf_lexer_peek(lexer);
    lexer->cursor++;
    return c;
}








 












