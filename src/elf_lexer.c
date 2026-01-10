#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <elf_lexer.h>
#include <elf_token.h>
#include <elf_utils.h>

#define STRING_QUOTE '"'
#define DECIMAL_CHAR '.'

typedef struct elf_tok_def elf_tok_def; 

struct elf_tok_def 
{
    union
    { 
        const char* str;
        const char chr;
    } value;
    elf_token_type tok_type;
};

typedef struct
{
    const char tok_value_left;
    const char tok_value_right;
    elf_token_type tok_type;
} cmpd_punct_token_def;

static const elf_tok_def kwd_table[] = 
{ 
   { .value.str = "null"  ,  TOK_KWD_NULL   }, 
   { .value.str = "bool"  ,  TOK_KWD_BOOL   },  
   { .value.str = "true"  ,  TOK_KWD_TRUE   },
   { .value.str = "false" ,  TOK_KWD_FALSE  },
   { .value.str = "int"   ,  TOK_KWD_INT    },   
   { .value.str = "float" ,  TOK_KWD_FLOAT  }, 
   { .value.str = "str"   ,  TOK_KWD_STR    },  
   { .value.str = "byte"  ,  TOK_KWD_BYTE   }, 
   { .value.str = "if"    ,  TOK_KWD_IF     }, 
   { .value.str = "else"  ,  TOK_KWD_ELSE   }, 
   { .value.str = "for"   ,  TOK_KWD_FOR    }, 
   { .value.str = "while" ,  TOK_KWD_WHILE  }, 
};


static const elf_tok_def punct_table[] = 
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

static const cmpd_punct_token_def cmpd_punct_table[] = 
{
    {'=', '=', TOK_EQ     }, {'!', '=', TOK_NEQ   },
    {'<', '=', TOK_LTE    }, {'>', '=', TOK_GTE   },
    {'&', '&', TOK_AND    }, {'|', '|', TOK_OR    },
    {'+', '=', TOK_PLUSEQ }, {'-', '=', TOK_MINEQ },
    {'*', '=', TOK_MULEQ  }, {'/', '=', TOK_DIVEQ },
    {'%', '=', TOK_MODEQ  }, {'*', '*', TOK_POW   },
};

static void elf_lexer_emit_token(elf_lexer* lexer, size_t origin, size_t len, elf_token_type type);
static bool elf_lexer_is_digit(char c);
static bool elf_lexer_is_ident(char c);
static bool elf_lexer_is_whitespace(char c);

static bool elf_lexer_try_scan_str_lit(elf_lexer* lexer);
static bool elf_lexer_try_scan_num_lit(elf_lexer* lexer);
static bool elf_lexer_try_scan_ident(elf_lexer* lexer);
static bool elf_lexer_try_scan_punct(elf_lexer* lexer);
static bool elf_lexer_try_scan_line_com(elf_lexer* lexer);
static bool elf_lexer_try_scan_block_com(elf_lexer* lexer);
static bool elf_lexer_try_scan_whitespace(elf_lexer* lexer);

static bool elf_lexer_try_get_kwd_type(const char* s, size_t len, elf_token_type* out_tok_type);
static bool elf_lexer_try_get_punct_type(char c, elf_token_type* out_tok_type);
static bool elf_lexer_try_get_cmpd_punct_type(char left, char right, elf_token_type* out_tok_type);

static char elf_lexer_consume(elf_lexer* lexer);
static char elf_lexer_peek(elf_lexer* lexer);
static char elf_lexer_peek_next(elf_lexer* lexer);

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
    lexer->token_vec = vec_create(64);
    if(!lexer->token_vec)
    {
        lexer->token_vec = NULL;
        fprintf(stderr, "error: could not allocate token-array memory\n");
        return (elf_lexer*){0};
    }

    printf("successful!\n");
    return lexer;
}

void elf_lexer_full_dispose(elf_lexer* lexer)
{
    printf("full disposing lexer...");
    for(int i = 0; i < lexer->token_vec->count; i++)
    {
        elf_token* tok = vec_get(lexer->token_vec, i);
        if(tok) free(tok);
    }
    vec_dispose(lexer->token_vec);
    free(lexer);
    printf("successful!\n");
}

void elf_lexer_emit_token(elf_lexer* lexer, size_t origin, size_t len, elf_token_type type)
{
    static size_t call_count = 0;
    call_count++;
    elf_token* token = elf_token_create(lexer->source, origin, len, type); 
    if(vec_append(lexer->token_vec, token) == false)
    {
        fprintf(stderr, "error: could not add new token to lexer->token_vec.\n");
        return;
    }
}

bool elf_lexer_tokenize(elf_lexer* lexer)
{
    printf("tokenizing...\n");
    if(!lexer || !lexer->token_vec) 
    {
        fprintf(stderr, "error: cannot tokenize, lexer or lexer->token_vec is invalid.\n");
        return false;
    }

    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        //omissions : scan-prio > all
        if      (elf_lexer_try_scan_whitespace(lexer))  continue; 
        else if (elf_lexer_try_scan_line_com(lexer))    continue;    
        else if (elf_lexer_try_scan_block_com(lexer))   continue;      
        
        else if (elf_lexer_try_scan_ident(lexer))       continue;   
        else if (elf_lexer_try_scan_num_lit(lexer))     continue; 
        else if (elf_lexer_try_scan_str_lit(lexer))     continue;
        else if (elf_lexer_try_scan_punct(lexer))       continue;   

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

bool elf_lexer_try_get_punct_type(char c, elf_token_type* out_tok_type) 
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

bool elf_lexer_try_get_cmpd_punct_type(char left, char right, elf_token_type* out_tok_type) 
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

bool elf_lexer_try_scan_str_lit(elf_lexer* lexer)
{
    if(elf_lexer_peek(lexer) != STRING_QUOTE)
        return false;

    elf_lexer_consume(lexer);
    size_t origin = lexer->cursor;
    
    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        elf_lexer_consume(lexer);
        if(elf_lexer_peek(lexer) == STRING_QUOTE)
        {
            elf_lexer_emit_token(lexer, origin, lexer->cursor - origin, TOK_STR_LIT);
            elf_lexer_consume(lexer);
            return true;
        }
    }
    return false;
}

bool elf_lexer_try_scan_num_lit(elf_lexer* lexer)
{
    bool float_flag = false;
    size_t origin = lexer->cursor;
    if(!elf_lexer_is_digit(elf_lexer_peek(lexer)))
    {
        if(elf_lexer_peek(lexer) != DECIMAL_CHAR || !elf_lexer_is_digit(elf_lexer_peek_next(lexer)) )
            return false;
        else
        {
            float_flag = true;
            elf_lexer_consume(lexer);
        }
    }

    elf_lexer_consume(lexer);
    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        if(elf_lexer_is_digit(elf_lexer_peek(lexer)))
        {
            elf_lexer_consume(lexer);
            continue;
        }
        else if(elf_lexer_peek(lexer) == DECIMAL_CHAR)
        {
            if(float_flag) break;
            float_flag = true;
            elf_lexer_consume(lexer);
        }
        else break;

    }
    elf_token_type tok_type = float_flag ? TOK_FLOAT_LIT : TOK_INT_LIT;
    elf_lexer_emit_token(lexer, origin, lexer->cursor - origin, tok_type);
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
       || elf_lexer_is_digit(elf_lexer_peek(lexer)))
    {
        elf_lexer_consume(lexer);
    }
    elf_token_type tok_type = TOK_IDENT;
    elf_lexer_try_get_kwd_type(lexer->source + origin, lexer->cursor - origin, &tok_type);
    elf_lexer_emit_token(lexer, origin, lexer->cursor - origin, tok_type); 
    return true;
}

bool elf_lexer_try_scan_punct(elf_lexer* lexer)
{
    size_t origin = lexer->cursor;
    char left = elf_lexer_peek(lexer);

    elf_token_type emit_tok_type = TOK_INV;
    if(!elf_lexer_try_get_punct_type(left, &emit_tok_type))
    {
        return false;
    } 
    elf_lexer_consume(lexer);
    char right = elf_lexer_peek(lexer);
    if(elf_lexer_try_get_cmpd_punct_type(left, right, &emit_tok_type))
    {
        elf_lexer_consume(lexer);
    }
    elf_lexer_emit_token(lexer, origin, lexer->cursor - origin, emit_tok_type);
    return true;
}

bool elf_lexer_try_scan_line_com(elf_lexer* lexer)
{
    if(elf_lexer_peek(lexer) != '/' || elf_lexer_peek_next(lexer) != '/')
    {
        return false;
    }
    elf_lexer_consume(lexer);
    elf_lexer_consume(lexer);
    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        if(elf_lexer_peek(lexer) == '\n')
        {
            elf_lexer_consume(lexer);
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
    elf_lexer_consume(lexer);
    elf_lexer_consume(lexer);
    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        if(elf_lexer_peek(lexer) == '*' && elf_lexer_peek_next(lexer) == '/')
        {
            elf_lexer_consume(lexer);
            elf_lexer_consume(lexer);
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
        {
            elf_lexer_consume(lexer);
        }
        return true;
    }
    return false;
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








 












