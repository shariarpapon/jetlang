#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <jet_lexer.h>
#include <jet_token.h>
#include <jet_io.h>

#define STRING_QUOTE '"'
#define CHAR_QUOTE '\''
#define DECIMAL_CHAR '.'
#define ESCAPE_CHAR '\\'

#define INIT_TOK_CAPACITY 32

typedef struct jet_tok_def 
{
    union
    { 
        const char* str;
        const char chr;
    } value;
    jet_token_type tok_type;
} jet_tok_def;

typedef struct
{
    const char tok_value_left;
    const char tok_value_right;
    jet_token_type tok_type;
} cmpd_punct_tok_def;

static const jet_tok_def kwd_table[] = 
{ 
   { .value.str = "prog"  ,  TOK_KWD_PROG     }, 
   { .value.str = "needs" ,  TOK_KWD_NEEDS    },  
   { .value.str = "null"  ,  TOK_KWD_NULL     }, 
   { .value.str = "bool"  ,  TOK_KWD_BOOL     },  
   { .value.str = "true"  ,  TOK_KWD_TRUE     },
   { .value.str = "false" ,  TOK_KWD_FALSE    },
   { .value.str = "int"   ,  TOK_KWD_INT      },   
   { .value.str = "float" ,  TOK_KWD_FLOAT    }, 
   { .value.str = "str"   ,  TOK_KWD_STR      },  
   { .value.str = "byte"  ,  TOK_KWD_BYTE     }, 
   { .value.str = "void"  ,  TOK_KWD_VOID     }, 
   { .value.str = "char"  ,  TOK_KWD_CHAR     }, 
   { .value.str = "mem"   ,  TOK_KWD_MEM      }, 
   { .value.str = "if"    ,  TOK_KWD_IF       }, 
   { .value.str = "else"  ,  TOK_KWD_ELSE     }, 
   { .value.str = "for"   ,  TOK_KWD_FOR      }, 
   { .value.str = "while" ,  TOK_KWD_WHILE    }, 
   { .value.str = "return",  TOK_KWD_RETURN   },
   { .value.str = "break" ,  TOK_KWD_BREAK    },
};

static const jet_tok_def punct_table[] = 
{
    { .value.chr = '=' , TOK_ASG   },    
    { .value.chr = '&' , TOK_BAND  },
    { .value.chr = '|' , TOK_BOR   },
    { .value.chr = '^' , TOK_XOR   },
    { .value.chr = '!' , TOK_NOT   },
    { .value.chr = '%' , TOK_MOD   },
    { .value.chr = '<' , TOK_LT    },  { .value.chr = '>' , TOK_GT    },      
    { .value.chr = '+' , TOK_PLUS  },  { .value.chr = '-' , TOK_MINUS }, 
    { .value.chr = '*' , TOK_STAR  },  { .value.chr = '/' , TOK_SLASH },  
    { .value.chr = ':' , TOK_COLON },  { .value.chr = ',' , TOK_COMMA }, 
    { .value.chr = ';' , TOK_SEMI  },  { .value.chr = '.' , TOK_DOT   },
    { .value.chr = '(' , TOK_LPAR  },  { .value.chr = ')' , TOK_RPAR  },
    { .value.chr = '{' , TOK_LBRC  },  { .value.chr = '}' , TOK_RBRC  },
    { .value.chr = '[' , TOK_LBRK  },  { .value.chr = ']' , TOK_RBRK  },
};

static const cmpd_punct_tok_def cmpd_punct_table[] = 
{
    {'=', '=', TOK_EQ     }, {'!', '=', TOK_NEQ   },
    {'<', '=', TOK_LTE    }, {'>', '=', TOK_GTE   },
    {'&', '&', TOK_AND    }, {'|', '|', TOK_OR    },
    {'+', '=', TOK_PLUSEQ }, {'-', '=', TOK_MINEQ },
    {'*', '=', TOK_MULEQ  }, {'/', '=', TOK_DIVEQ },
    {'%', '=', TOK_MODEQ  }, {'*', '*', TOK_POW   },
    {'+', '+', TOK_INCR   }, {'-', '-', TOK_DECR  }, 
    {'<', '<', TOK_SHL    }, {'>', '>', TOK_SHR   },
    {'&', '=', TOK_BANDEQ }, {'|', '=', TOK_BOREQ },
    {'^', '=', TOK_XOREQ  },
};

static void jet_lexer_emit_token(jet_lexer* lexer, size_t origin, size_t len, jet_token_type tok_type);
static bool jet_lexer_is_digit(char c);
static bool jet_lexer_is_ident(char c);

static bool jet_lexer_try_scan_str_lit(jet_lexer* lexer);
static bool jet_lexer_try_scan_num_lit(jet_lexer* lexer);
static bool jet_lexer_try_scan_char_lit(jet_lexer* lexer);
static bool jet_lexer_try_scan_ident(jet_lexer* lexer);
static bool jet_lexer_try_scan_punct(jet_lexer* lexer);
static bool jet_lexer_try_scan_line_com(jet_lexer* lexer);
static bool jet_lexer_try_scan_block_com(jet_lexer* lexer);
static bool jet_lexer_try_scan_whitespace(jet_lexer* lexer);

static bool jet_lexer_try_get_kwd_type(const char* s, size_t len, jet_token_type* out_tok_type);
static bool jet_lexer_try_get_punct_type(char c, jet_token_type* out_tok_type);
static bool jet_lexer_try_get_cmpd_punct_type(char left, char right, jet_token_type* out_tok_type);

static char jet_lexer_consume(jet_lexer* lexer);
static char jet_lexer_peek(jet_lexer* lexer);
static char jet_lexer_peek_next(jet_lexer* lexer);

jet_lexer* jet_lexer_create(const char* filename)
{
    if(!filename)
    {
        fprintf(stderr, "error: cannot create lexer, filename invalid.\n");
        return NULL;
    }

    size_t src_len = 0;
    const char* source = jet_io_read_text(filename, &src_len);
    if(!source)
    {
        fprintf(stderr, "error: cannot create lexer, unable to load file.\n");
        return NULL;
    }

    if(src_len == 0)
    {
        fprintf(stderr, "wrn: loaded file is empty.\n");
        free((void*)source);
        return NULL;
    }

    jet_lexer* lexer = malloc(sizeof(jet_lexer));
    if(!lexer)
    {
        fprintf(stderr, "error: could not allocate lexer memory\n");
        free((void*)source);
        return NULL;
    }

    lexer->source = source;
    lexer->len = src_len + 1;
    lexer->cursor = 0;
    lexer->cur_line = 0;
    lexer->token_darray = jet_da_create(INIT_TOK_CAPACITY, sizeof(jet_token));
    if(!lexer->token_darray)
    {
        lexer->token_darray = NULL;
        fprintf(stderr, "error: could not allocate token-darray memory\n");
        jet_lexer_dispose(lexer);
        return NULL;
    }

    printf("lexer created successfully!\n");
    return lexer;
}

void jet_lexer_dispose(jet_lexer* lexer)
{
    if(!lexer) 
    {
        perror("error: attempting to free invalid lexer pointer.\n");
        return;
    }

    if(lexer->source)
    {
        free((void*)lexer->source);
    }

    if(!lexer->token_darray)
    {
        free((void*)lexer);
        return;
    }
    jet_da_dispose(lexer->token_darray);
    free((void*)lexer);
    printf("lexer disposed!\n");
}

bool jet_lexer_tokenize(jet_lexer* lexer)
{
    printf("tokenizing...\n");
    if(!lexer || !lexer->token_darray) 
    {
        fprintf(stderr, "error: cannot tokenize, lexer or lexer->token_darray is invalid.\n");
        return false;
    }
    //TEST
    while(true)
    {
        if(jet_lexer_peek(lexer) == NULL_TERM)
        {
            jet_lexer_emit_token(lexer, lexer->cursor, 1, TOK_EOF);
            jet_lexer_consume(lexer);
            break;
        }
        else if (jet_lexer_try_scan_whitespace(lexer))  continue; 
        else if (jet_lexer_try_scan_line_com(lexer))    continue;    
        else if (jet_lexer_try_scan_block_com(lexer))   continue;      
        else if (jet_lexer_try_scan_ident(lexer))       continue;   
        else if (jet_lexer_try_scan_num_lit(lexer))     continue; 
        else if (jet_lexer_try_scan_char_lit(lexer))    continue;
        else if (jet_lexer_try_scan_str_lit(lexer))     continue;
        else if (jet_lexer_try_scan_punct(lexer))       continue;   
        
        jet_lexer_emit_token(lexer, lexer->cursor, 1, TOK_INV);
        jet_lexer_consume(lexer);
    } 
    printf("tokenization complete!\n");
    return true;
}

static void jet_lexer_emit_token(jet_lexer* lexer, size_t origin, size_t len, jet_token_type tok_type)
{
    jet_token tok;
    tok.source = lexer->source;
    tok.origin = origin;
    tok.len = len;
    tok.line = lexer->cur_line;
    tok.column = lexer->cursor - len;
    tok.type = tok_type;    

    if(jet_da_append(lexer->token_darray, (const void*)&tok) == false)
    {
        fprintf(stderr, "error: could not add new token to lexer->token_darray.\n");
        return;
    }
}

static bool jet_lexer_is_ident(char c)
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

static bool jet_lexer_is_digit(char c)
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

static bool jet_lexer_try_get_kwd_type(const char* s, size_t len, jet_token_type* out_tok_type)
{ 
    if(!out_tok_type)
    {
        perror("invalid output pointer");
        return false;
    }

    char token_str[len + 1];
    memcpy(token_str, (void*)s, len);
    token_str[len] = NULL_TERM;
    for(size_t i = 0; i < ARRAY_LEN(kwd_table); i++)
    {
       if(strcmp(token_str, kwd_table[i].value.str) == 0)
       {
            *out_tok_type = kwd_table[i].tok_type;
            return true;
       } 
    }

    return false;
}

static bool jet_lexer_try_get_punct_type(char c, jet_token_type* out_tok_type) 
{
    if(!out_tok_type)
    {
        perror("invalid output pointer");
        return false;
    }
    
    for(size_t i = 0; i < ARRAY_LEN(punct_table); i++)
    {
        if(punct_table[i].value.chr == c)
        {
            *out_tok_type = punct_table[i].tok_type;
            return true;
        }
    }
    return false; 
}

static bool jet_lexer_try_get_cmpd_punct_type(char left, char right, jet_token_type* out_tok_type) 
{
    if(!out_tok_type)
    {
        fprintf(stderr, "error: invalid output pointer\n");
        return false;
    }
    
    for(size_t i = 0; i < ARRAY_LEN(cmpd_punct_table); i++)
    {
        if(cmpd_punct_table[i].tok_value_right == right && 
           cmpd_punct_table[i].tok_value_left == left)
        {
            *out_tok_type = cmpd_punct_table[i].tok_type;
            return true;
        }
    }
    return false; 
}

static bool jet_lexer_try_scan_str_lit(jet_lexer* lexer)
{
    if(jet_lexer_peek(lexer) != STRING_QUOTE)
        return false;

    jet_lexer_consume(lexer);
    size_t origin = lexer->cursor;
    bool escaped = false;
    while(jet_lexer_peek(lexer) != NULL_TERM)
    {
        jet_lexer_consume(lexer);
        
        if(jet_lexer_peek(lexer) == STRING_QUOTE && !escaped)
        {
            jet_lexer_emit_token(lexer, origin, lexer->cursor - origin, TOK_LIT_STR);
            jet_lexer_consume(lexer);
            return true;
        }
        
        if(escaped) escaped = false;
        else if(jet_lexer_peek(lexer) == ESCAPE_CHAR) escaped = true;    
    }
    return false;
}

static bool jet_lexer_try_scan_num_lit(jet_lexer* lexer)
{
    bool float_flag = false;
    size_t origin = lexer->cursor;
    if(!jet_lexer_is_digit(jet_lexer_peek(lexer)))
    {
        if(jet_lexer_peek(lexer) != DECIMAL_CHAR || !jet_lexer_is_digit(jet_lexer_peek_next(lexer)) )
            return false;
        else
        {
            float_flag = true;
            jet_lexer_consume(lexer);
        }
    }

    jet_lexer_consume(lexer);
    while(jet_lexer_peek(lexer) != NULL_TERM)
    {
        if(jet_lexer_is_digit(jet_lexer_peek(lexer)))
        {
            jet_lexer_consume(lexer);
            continue;
        }
        else if(jet_lexer_peek(lexer) == DECIMAL_CHAR)
        {
            if(float_flag) break;
            float_flag = true;
            jet_lexer_consume(lexer);
        }
        else break;

    }
    jet_token_type tok_type = float_flag ? TOK_LIT_FLOAT : TOK_LIT_INT;
    jet_lexer_emit_token(lexer, origin, lexer->cursor - origin, tok_type);
    return true;
}

static bool jet_lexer_try_scan_char_lit(jet_lexer* lexer)
{
    if(jet_lexer_peek(lexer) != CHAR_QUOTE) 
        return false;
    
    jet_lexer_consume(lexer);
    if(jet_lexer_peek_next(lexer) != CHAR_QUOTE) 
        return false;

    jet_lexer_emit_token(lexer, lexer->cursor, 1, TOK_LIT_CHAR);
    jet_lexer_consume(lexer);
    jet_lexer_consume(lexer);
    return true;
}

static bool jet_lexer_try_scan_ident(jet_lexer* lexer)
{
    if(jet_lexer_is_ident(jet_lexer_peek(lexer)) == false)
    {
        return false;
    }

    size_t origin = lexer->cursor;
    jet_lexer_consume(lexer);

    while(jet_lexer_is_ident(jet_lexer_peek(lexer))
       || jet_lexer_is_digit(jet_lexer_peek(lexer)))
    {
        jet_lexer_consume(lexer);
    }
    jet_token_type tok_type = TOK_IDENT;
    jet_lexer_try_get_kwd_type(lexer->source + origin, lexer->cursor - origin, &tok_type);
    jet_lexer_emit_token(lexer, origin, lexer->cursor - origin, tok_type); 
    return true;
}

static bool jet_lexer_try_scan_punct(jet_lexer* lexer)
{
    size_t origin = lexer->cursor;
    char left = jet_lexer_peek(lexer);

    jet_token_type emit_tok_type = TOK_INV;
    if(!jet_lexer_try_get_punct_type(left, &emit_tok_type))
    {
        return false;
    } 
    jet_lexer_consume(lexer);
    char right = jet_lexer_peek(lexer);
    if(jet_lexer_try_get_cmpd_punct_type(left, right, &emit_tok_type))
    {
        jet_lexer_consume(lexer);
    }
    jet_lexer_emit_token(lexer, origin, lexer->cursor - origin, emit_tok_type);
    return true;
}

static bool jet_lexer_try_scan_line_com(jet_lexer* lexer)
{
    if(jet_lexer_peek(lexer) != '/' || jet_lexer_peek_next(lexer) != '/')
    {
        return false;
    }
    jet_lexer_consume(lexer);
    jet_lexer_consume(lexer);
    while(jet_lexer_peek(lexer) != NULL_TERM)
    {
        if(jet_lexer_peek(lexer) == '\n')
        {
            jet_lexer_consume(lexer);
            return true;        
        }
        jet_lexer_consume(lexer);
    }   
    return false;
}

static bool jet_lexer_try_scan_block_com(jet_lexer* lexer)
{
    if(jet_lexer_peek(lexer) != '/' || jet_lexer_peek_next(lexer) != '*')
    {
        return false;
    }
    jet_lexer_consume(lexer);
    jet_lexer_consume(lexer);
    while(jet_lexer_peek(lexer) != NULL_TERM)
    {
        if(jet_lexer_peek(lexer) == '*' && jet_lexer_peek_next(lexer) == '/')
        {
            jet_lexer_consume(lexer);
            jet_lexer_consume(lexer);
            return true;
        }
        jet_lexer_consume(lexer);
    }

    return false;
}

static bool jet_lexer_try_scan_whitespace(jet_lexer* lexer)
{
    switch(jet_lexer_peek(lexer))
    {
        case '\n': 
            lexer->cur_line++;
            break;

        case ' ' :
        case '\t':
        case '\v':
        case '\f':
        case '\r':
            break;

        default: 
            return false;
    }
    jet_lexer_consume(lexer);
    return true;
}

static char jet_lexer_peek_next(jet_lexer* lexer)
{
    size_t next = lexer->cursor + 1;
    if(next >= lexer->len - 1)
    {
        printf("wrn: cannot peek next, EOF reached\n");
        return NULL_TERM;
    }
    return lexer->source[next];
}

static char jet_lexer_peek(jet_lexer* lexer)
{
    return lexer->source[lexer->cursor];
}

static char jet_lexer_consume(jet_lexer* lexer)
{
    if(lexer->cursor >= lexer->len - 1)
    {
        fprintf(stderr, "wrn: cannot consume, EOF reached.\n");
        return NULL_TERM;
    }
    char c = jet_lexer_peek(lexer);
    lexer->cursor++;
    return c;
}








 












