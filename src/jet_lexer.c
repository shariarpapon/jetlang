#include <stdlib.h>
#include <jet_lexer.h>
#include <jet_token.h>
#include <jet_io.h>
#include <jet_logger.h>

#include <string.h>
#include <stdio.h>

#define STRING_QUOTE '"'
#define CHAR_QUOTE '\''
#define DECIMAL_CHAR '.'
#define ESCAPE_CHAR '\\'
#define TAB_WIDTH (4)

#define INIT_TOK_CAP 32

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

static void jet_lexer_emit_token(jet_lexer* lexer, size_t offset, size_t len, jet_token_type tok_type);
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

//lexer should be zero initialized, otherwise memory leak may occure if called with already initialized lexer.
bool jet_lexer_init(jet_lexer* lexer, const char* filename, const char* input, jet_da* token_da)
{
    JET_ASSERT(lexer != NULL);
    if(!input || !token_da)
    {
        JET_LOG_ERROR("cannot init, invalid param/s.");
        return false;
    }
    memset(lexer, 0, sizeof(*lexer)); 
    lexer->filename = filename;
    lexer->input = input;
    lexer->input_len = strlen(input);
    lexer->cursor = 0;
    lexer->cur_line = 1;
    lexer->cur_col = 1;
    lexer->token_da = token_da;
    return true;
}

void jet_lexer_reset(jet_lexer* lexer)
{
    JET_ASSERT(lexer != NULL);
    lexer->cursor = 0;
    lexer->cur_line = 1;
    lexer->cur_col = 1;
    jet_da_clear(lexer->token_da);
}

void jet_lexer_dispose(jet_lexer* lexer)
{
    if(!lexer) return;
    memset(lexer, 0, sizeof(*lexer));
}

bool jet_lexer_tokenize(jet_lexer* lexer)
{
    JET_ASSERT(lexer != NULL);
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
    return true;
}

static void jet_lexer_emit_token(jet_lexer* lexer, size_t start_cursor, size_t len, jet_token_type tok_type)
{
    JET_ASSERT(lexer != NULL);
    jet_token tok;
    const char* lexeme = lexer->input + start_cursor;
    if(!jet_token_init(
                &tok, 
                tok_type, 
                lexeme, 
                start_cursor, 
                start_cursor + len,
                lexer->cur_line, 
                lexer->cur_col - len))
    {
        JET_LOG_ERROR(" failed to emit token, could not init token.\n");
        return;
    }

    if(!jet_da_append(lexer->token_da, (const void*)&tok))
    {
        JET_LOG_ERROR(": failed to emit token, could not push token.\n");
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
    if(!s) 
        return false;
    if(!out_tok_type)
    {
        JET_LOG_ERROR("output token type pointer out_tok_type is null.");
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
        JET_LOG_ERROR("output token type pointer out_tok_type is null.");
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
        JET_LOG_ERROR("output token type pointer out_tok_type is null.");
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
    JET_ASSERT(lexer != NULL);
    if(jet_lexer_peek(lexer) != STRING_QUOTE)
        return false;

    size_t start_cursor = lexer->cursor;
    
    jet_lexer_consume(lexer);
    bool escaped = false;
    while(jet_lexer_peek(lexer) != NULL_TERM)
    {
        jet_lexer_consume(lexer);
        
        if(jet_lexer_peek(lexer) == STRING_QUOTE && !escaped)
        {
            jet_lexer_consume(lexer);
            jet_lexer_emit_token(lexer, start_cursor, lexer->cursor - start_cursor, TOK_LIT_STR);
            return true;
        }
        
        if(escaped) escaped = false;
        else if(jet_lexer_peek(lexer) == ESCAPE_CHAR) escaped = true;    
    }
    return false;
}

static bool jet_lexer_try_scan_num_lit(jet_lexer* lexer)
{
    JET_ASSERT(lexer != NULL);
    bool float_flag = false;
    size_t start_cursor = lexer->cursor;
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
    jet_lexer_emit_token(lexer, start_cursor, lexer->cursor - start_cursor, tok_type);
    return true;
}

static bool jet_lexer_try_scan_char_lit(jet_lexer* lexer)
{
    JET_ASSERT(lexer != NULL);
    if(jet_lexer_peek(lexer) != CHAR_QUOTE) 
        return false;
    size_t start_cursor = lexer->cursor;
    jet_lexer_consume(lexer);
    if(jet_lexer_peek_next(lexer) != CHAR_QUOTE) 
        return false;
    jet_lexer_consume(lexer);
    jet_lexer_consume(lexer);
    jet_lexer_emit_token(lexer, start_cursor, 3, TOK_LIT_CHAR);
    return true;
}

static bool jet_lexer_try_scan_ident(jet_lexer* lexer)
{
    JET_ASSERT(lexer != NULL);
    if(jet_lexer_is_ident(jet_lexer_peek(lexer)) == false)
    {
        return false;
    }
    size_t start_cursor = lexer->cursor;
    jet_lexer_consume(lexer);
    while(jet_lexer_is_ident(jet_lexer_peek(lexer))
       || jet_lexer_is_digit(jet_lexer_peek(lexer)))
    {
        jet_lexer_consume(lexer);
    }
    jet_token_type tok_type = TOK_IDENT;
    jet_lexer_try_get_kwd_type(lexer->input + start_cursor, lexer->cursor - start_cursor, &tok_type);
    jet_lexer_emit_token(lexer, start_cursor, lexer->cursor - start_cursor, tok_type); 
    return true;
}

static bool jet_lexer_try_scan_punct(jet_lexer* lexer)
{
    JET_ASSERT(lexer != NULL);
    char left = jet_lexer_peek(lexer);
    jet_token_type emit_tok_type = TOK_INV;
    if(!jet_lexer_try_get_punct_type(left, &emit_tok_type))
        return false;
    size_t start_cursor = lexer->cursor;
    jet_lexer_consume(lexer);
    char right = jet_lexer_peek(lexer);
    if(jet_lexer_try_get_cmpd_punct_type(left, right, &emit_tok_type))
        jet_lexer_consume(lexer);
    jet_lexer_emit_token(lexer, start_cursor, lexer->cursor - start_cursor, emit_tok_type);
    return true;
}

static bool jet_lexer_try_scan_line_com(jet_lexer* lexer)
{
    JET_ASSERT(lexer != NULL);
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
    JET_ASSERT(lexer != NULL);
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
    JET_ASSERT(lexer != NULL);
    switch(jet_lexer_peek(lexer))
    {
        case '\n':
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
    JET_ASSERT(lexer != NULL);
    size_t next = lexer->cursor + 1;
    if(next >= lexer->input_len)
        return NULL_TERM;
    return lexer->input[next];
}

static char jet_lexer_peek(jet_lexer* lexer)
{
    JET_ASSERT(lexer != NULL);
    return lexer->input[lexer->cursor];
}

static char jet_lexer_consume(jet_lexer* lexer)
{
    JET_ASSERT(lexer != NULL);
    if(lexer->cursor >= lexer->input_len)
        return NULL_TERM;
    char c = jet_lexer_peek(lexer);
    lexer->cursor++;
    if(c == '\t')
        lexer->cur_col += TAB_WIDTH - ((lexer->cur_col  - 1) % TAB_WIDTH);
    else if(c == '\n')
    {
        lexer->cur_line++;
        lexer->cur_col = 1;
    }
    else lexer->cur_col++;
    return c;
}








 












