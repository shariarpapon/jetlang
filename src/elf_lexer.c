#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include <elf_lexer.h>
#include <elf_token.h>
#include <elf_utils.h>

elf_lexer* elf_lexer_create(const char* source)
{
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

    const size_t tok_capacity = 128;
    lexer->tokens = tblist_create(tok_capacity);

    if(!lexer->tokens)
    {
        lexer->tokens = NULL;
        fprintf(stderr, "error: could not allocate token-array memory\n");
        return (elf_lexer*){0};
    }

    printf("lexer created...\n");

    return lexer;
}

void elf_lexer_full_dispose(elf_lexer* lexer)
{
    free(lexer->tokens);
    free(lexer);
}

void elf_lexer_emit_token(elf_lexer* lexer, size_t origin, size_t len, elf_token_type type)
{
    elf_token* token = elf_token_create(lexer->source, origin, len, type);
    if(tblist_append(lexer->tokens, token) == FALSE)
    {
        fprintf(stderr, "error: could not add new token to lexer->tokens.\n");
        return;
    }
}

BOOL elf_lexer_tokenize(elf_lexer* lexer)
{
    if(!lexer || !lexer->tokens) 
    {
        fprintf(stderr, "error: cannot tokenize, lexer or lexer->tokens is invalid.\n");
        return FALSE;
    }
    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        if(elf_lexer_scan_line_com(lexer) == TRUE)
            continue;
        else if(elf_lexer_scan_block_com(lexer) == TRUE)
           continue;
        else if(elf_lexer_scan_whitepace(lexer) == TRUE)
            continue;
        else if(elf_lexer_scan_ident(lexer)== TRUE)
            continue;
        else if(elf_lexer_scan_symbol(lexer) == TRUE)
            continue;
        else if (elf_lexer_scan_num(lexer)== TRUE)
            continue;
        else if(elf_lexer_scan_opr(lexer) == TRUE)
           continue;

        elf_lexer_emit_token(lexer, lexer->cursor, 1, TOK_INV);
        elf_lexer_advance(lexer);
    }
    return TRUE;
}

BOOL elf_lexer_is_ident(char c)
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
            return TRUE;
        default:
            return FALSE;
    }
    return FALSE;
}

BOOL elf_lexer_is_digit(char c)
{
    switch (c)
    {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return TRUE;
        default:
            return FALSE;
    }
    return FALSE;
}

BOOL elf_lexer_is_whitespace(char c)
{
    BOOL is_whitespace =  
            c == ' ' || 
            c == '\n' || 
            c == '\t' || 
            c == '\f' || 
            c == '\r' || 
            c == '\v';
    return is_whitespace;
}

elf_token_type elf_lexer_eval_opr_type(char c, BOOL* succ)
{
    *succ = TRUE;
    switch(c)
    {
        case '=': return TOK_ASG;
        case '>': return TOK_GT;
        case '<': return TOK_LT; 
        case '!': return TOK_NOT;
        case '&': return TOK_BAND;
        case '^': return TOK_XOR;
        case '|': return TOK_BOR;
        case '+': return TOK_PLUS;
        case '-': return TOK_MINUS;
        case '*': return TOK_STAR;
        case '/': return TOK_SLASH;
        case '%': return TOK_MOD;
    }
    *succ = FALSE; 
    return TOK_INV;
}

elf_token_type elf_lexer_eval_cmpd_opr_type(elf_token_type l, elf_token_type r, BOOL* succ)
{
    if(!succ)
    {
        fprintf(stderr, "error: output status BOOL 'success' does not have valid allocation.\n");
        return TOK_INV;
    }

    *succ = TRUE;
    if (r == TOK_ASG) 
    {
        if (l == TOK_ASG)          {  return TOK_EQ; }
        else if (l == TOK_NOT)     {  return TOK_NEQ; }
        else if (l == TOK_PLUS)    {  return TOK_PLUSEQ; }
        else if (l == TOK_MINUS)   {  return TOK_MINEQ; }
        else if (l == TOK_STAR)    {  return TOK_MULEQ; }
        else if (l == TOK_SLASH)   {  return TOK_DIVEQ; }
        else if (l == TOK_MOD)     {  return TOK_MODEQ; }
        else if (l == TOK_GT)      {  return TOK_GTE; }
        else if (l == TOK_LT)      {  return TOK_LTE; }
    }
    else if (l == TOK_BOR && r == TOK_BOR) { *succ = TRUE; return TOK_OR; }
    else if (l == TOK_BAND && r == TOK_BAND) { *succ = TRUE; return TOK_AND; }
    else if (l == TOK_STAR && r == TOK_STAR) { *succ = TRUE; return TOK_POW; }  
    
    *succ = FALSE;
    return TOK_INV;
}

char elf_lexer_consume(elf_lexer* lexer)
{
    char c = elf_lexer_peek(lexer);
    elf_lexer_advance(lexer);
    return c;
}

BOOL elf_lexer_scan_ident(elf_lexer* lexer)
{
    if(elf_lexer_is_ident(elf_lexer_peek(lexer)) == FALSE)
    {
        return FALSE;
    }

    size_t origin = lexer->cursor;
    elf_lexer_advance(lexer);

    while(elf_lexer_is_ident(elf_lexer_peek(lexer))
       || elf_lexer_is_digit(elf_lexer_peek(lexer)) )
    {
        elf_lexer_advance(lexer);
    }

    size_t len = lexer->cursor - origin;
    elf_lexer_emit_token(lexer, origin, len, TOK_IDENT);
    return len != 0;
}

BOOL elf_lexer_scan_num(elf_lexer* lexer)
{
    if(elf_lexer_is_digit(elf_lexer_peek(lexer)) == FALSE)
    {
        return FALSE;
    }

    size_t origin = lexer->cursor;
    elf_lexer_advance(lexer); 
    while(elf_lexer_is_digit(elf_lexer_peek(lexer)))
    {
        elf_lexer_advance(lexer);
    }
    size_t len = lexer->cursor - origin;
    elf_lexer_emit_token(lexer, origin, len, TOK_NUM);
    return len != 0;
}


BOOL elf_lexer_scan_line_com(elf_lexer* lexer)
{
    if(elf_lexer_peek(lexer) != '/' || elf_lexer_peek_next(lexer) != '/')
    {
        return FALSE;
    }

    size_t origin = lexer->cursor;
    elf_lexer_advance(lexer);
    elf_lexer_advance(lexer);

    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        if(elf_lexer_peek(lexer) == '\n' || 
           elf_lexer_peek(lexer) == '\r')
        {
            elf_lexer_advance(lexer);
            size_t len = lexer->cursor - origin;
            elf_lexer_emit_token(lexer, origin, len, TOK_LCOM);
            return TRUE;        
        }
        elf_lexer_advance(lexer);
    }   
    return FALSE;
}

BOOL elf_lexer_scan_block_com(elf_lexer* lexer)
{
    if(elf_lexer_peek(lexer) != '/' || elf_lexer_peek_next(lexer) != '*')
    {
        return FALSE;
    }

    size_t origin = lexer->cursor;
    elf_lexer_advance(lexer);
    elf_lexer_advance(lexer);
    while(elf_lexer_peek(lexer) != NULL_TERM)
    {
        if(elf_lexer_peek(lexer) == '*' && elf_lexer_peek_next(lexer) == '/')
        {
            elf_lexer_advance(lexer);
            elf_lexer_advance(lexer);
            size_t len = lexer->cursor - origin;
            elf_lexer_emit_token(lexer, origin, len, TOK_BCOM);
            return TRUE;
        }
        elf_lexer_advance(lexer);
    }

    return FALSE;
}

BOOL elf_lexer_scan_opr(elf_lexer* lexer)
{
    size_t len = 0;
    size_t origin = lexer->cursor;
    char curr = elf_lexer_peek(lexer);
    char next = elf_lexer_peek_next(lexer);
    BOOL succ = FALSE;

    elf_token_type oprType = elf_lexer_eval_opr_type(curr, &succ);
    if(succ != TRUE) 
        return FALSE;
 
    elf_lexer_advance(lexer);
    len++;
    
    succ = FALSE;
    elf_token_type nextOprType = elf_lexer_eval_opr_type(next, &succ);
    if(succ == TRUE)
    {
        succ = FALSE;
        elf_token_type cmpdType = elf_lexer_eval_cmpd_opr_type(oprType, nextOprType, &succ);
        if(succ == TRUE)
        {
            elf_lexer_advance(lexer);
            len++;
            oprType = cmpdType;   
        }
    }   
    elf_lexer_emit_token(lexer, origin, len, oprType);
    return len != 0;
}

BOOL elf_lexer_scan_whitepace(elf_lexer* lexer)
{
    if(elf_lexer_is_whitespace(elf_lexer_peek(lexer)) == FALSE)
        return FALSE;
    elf_lexer_advance(lexer);
    while(elf_lexer_is_whitespace(elf_lexer_peek(lexer)) == TRUE)
        elf_lexer_advance(lexer);
    return TRUE;
}

BOOL elf_lexer_scan_symbol(elf_lexer* lexer)
{
    size_t origin = lexer->cursor;
    elf_token_type type = TOK_INV;
    switch(elf_lexer_peek(lexer))
    {
        default: return FALSE;
        case ';': type = TOK_SEMI;  break;
        case '.': type = TOK_DOT ;  break;
        case '(': type = TOK_LPAR;  break;
        case ')': type = TOK_RPAR;  break;
        case '{': type = TOK_LBRC;  break;
        case '}': type = TOK_RBRC;  break;
        case '[': type = TOK_LBRK;  break;
        case ']': type = TOK_RBRK;  break;
        case ',': type = TOK_COMMA; break;
    }
    elf_lexer_advance(lexer);
    elf_lexer_emit_token(lexer, origin, 1, type);
    return TRUE;
}

char elf_lexer_peek_prev(elf_lexer* lexer)
{
    size_t last = lexer->cursor - 1;
    if(last < 0)
    {
        printf("wrn: cannot neek prev, index < 0... returning null-term\n");
        return NULL_TERM;
    }
    return lexer->source[last];
}

char elf_lexer_peek_next(elf_lexer* lexer)
{
    size_t next = lexer->cursor + 1;
    if(next >= lexer->cursor)
    {
        printf("wrn: cannot peek next, EOF reached... returning null-term\n");
        return NULL_TERM;
    }
    return lexer->source[next];
}

char elf_lexer_peek(elf_lexer* lexer)
{
    return lexer->source[lexer->cursor];
}

char elf_lexer_try_peek_ahead(elf_lexer* lexer, size_t n, BOOL* succ)
{
    size_t i = lexer->cursor + n;
    if(i >= lexer->len)
    {
        *succ = FALSE; 
        return NULL_TERM;
    }
    *succ = TRUE;
    return lexer->source[i];
}

void elf_lexer_advance(elf_lexer* lexer)
{
    lexer->cursor++;
}








 












