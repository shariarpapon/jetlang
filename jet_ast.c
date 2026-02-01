#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <jet_ast.h>
#include <jet_ast_node.h>
#include <jet_ast_node_create.h>
#include <jet_ast_op_prec.h>

struct jet_ast 
{
    jet_list* tok_list;
    jet_list* top_node_list;
    jet_ast_node* prog_node;
    size_t tok_cursor;
};

static void jet_ast_add_top_node(jet_ast* ast, jet_ast_node* node);
static bool jet_ast_generate(jet_ast* ast);
static jet_ast_node* jet_ast_get_next_node(jet_ast* ast);
static size_t jet_ast_get_type_byte_size(jet_token_type tok_type);

static jet_ast_node* jet_ast_node_prog_parse(jet_ast* ast);
static jet_ast_node* jet_ast_node_mem_parse(jet_ast* ast);
static jet_ast_node* jet_ast_node_block_parse(jet_ast* ast);
static jet_ast_node* jet_ast_node_lit_parse(jet_ast* ast);
static jet_ast_node* jet_ast_node_tdecl_parse(jet_ast* ast);
static jet_ast_node* jet_ast_node_ident_parse(jet_ast* ast);
static jet_ast_node* jet_ast_node_vdecl_parse(jet_ast* ast);

static jet_list* jet_ast_node_func_parse_params(jet_ast* ast, bool* out_defines_func);
static jet_ast_node* jet_ast_node_parse_ctrl_stmt(jet_ast* ast);
static jet_ast_node* jet_ast_node_parse_expr(jet_ast* ast, size_t min_prec);
static jet_ast_node* jet_ast_node_parse_primary(jet_ast* ast);
static jet_ast_node* jet_ast_node_tok_value_ident(jet_token* tok);

static jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type);
static jet_token* jet_ast_peek_tok(jet_ast* ast);
static jet_token* jet_ast_peek_next_tok(jet_ast* ast);
static jet_token* jet_ast_consume_tok(jet_ast* ast);

static size_t jet_ast_get_op_prec(jet_token_type op_type)
{
    switch(op_type) {

        case TOK_INCR:
        case TOK_DECR:
        case TOK_DOT: return PREC_POSTFIX;
        case TOK_NOT: return PREC_PREFIX;
        
        case TOK_STAR:
        case TOK_SLASH:
        case TOK_MOD: return PREC_MULT;
        
        case TOK_PLUS:
        case TOK_MINUS: return PREC_ADDITIVE;
        
        case TOK_SHL:
        case TOK_SHR: return PREC_BIT_SHIFT;
        
        case TOK_BAND: return PREC_BAND;
        case TOK_BOR: return PREC_BOR;
        case TOK_AND: return PREC_AND;
        case TOK_OR: return PREC_OR;
        
        case TOK_GTE:
        case TOK_LTE:
        case TOK_GT:
        case TOK_LT: return PREC_RELATIONAL;

        case TOK_PLSUEQ:
        case TOK_MINEQ:
        case TOK_MULEQ:
        case TOK_DIVEQ:
        case TOK_MODEQ:
        case TOK_XOREQ:
        case TOK_BANDEQ:
        case TOK_BOREQ: return PREC_ASG;
        
        case TOK_POW: return PREC_POW;


        default: return 0; // not an operator
    }
}


jet_ast* jet_ast_create(jet_list* tok_list)
{    
    if(!tok_list)
    {
        fprintf(stderr, "error: cannot create ast, invalid token list.\n");
        return NULL;
    }

    jet_list* top_node_list = jet_list_create(2, sizeof(jet_ast_node));
    if(!top_node_list)
    {
        fprintf(stderr, "error: cannot create ast, could not create node list.\n");
        return NULL;
    }

    jet_ast* ast =  (jet_ast*)malloc(sizeof(jet_ast));
    if(!ast)
    {
        fprintf(stderr, "error: cannot create ast, failed to allocate memory.\n");
        return NULL;
    }

    ast->tok_list = tok_list;
    ast->top_node_list = top_node_list; 
    ast->prog_node = NULL;
    ast->tok_cursor = 0;
    return ast;
}

bool jet_ast_dispose(jet_ast* ast)
{
    if(!ast)
    {
        fprintf(stderr, "error: cannot free, given jet_ast is NULL.\n");
        return false;
    }

    for(size_t i = 0; i < jet_list_count(ast->top_node_list); i++)
    {
        void* node = jet_list_get(ast->top_node_list, i);
        if(node)
            jet_ast_node_dispose(node);
    }

    if(ast->top_node_list) jet_list_dispose(ast->top_node_list); 
    if(ast->prog_node) jet_ast_node_dispose(ast->prog_node);
    free(ast);
    return true;
}

static bool jet_ast_generate(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    if(jet_list_count(ast->tok_list) == 0)
    {
        fprintf(stderr, "erorr: cannot generate AST, token-list is empty.\n");
        return false;
    }

    jet_ast_node* cur_node = jet_ast_get_next_node(ast);
    while(cur_node  != NULL)
    {
        if(cur_node->node_type == AST_PROG)
        {
            if(ast->prog_node != NULL)
            {
                fprintf(stderr, "error: cannot generate AST, more than 1 program entry point found.\n");
                return false;
            }
            ast->prog_node = cur_node;
        }
        else jet_ast_add_top_node(ast, cur_node);        
        cur_node = jet_ast_get_next_node(ast);
    }
    return true;
}

static jet_ast_node* jet_ast_get_next_node(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    size_t tok_count = jet_list_count(ast->tok_list);
    if(tok_count == 0)
    {
        fprintf(stderr, "wrn: cannot eval node, token-list is empty.\n");
        return NULL;
    }
    if(ast->tok_cursor >= tok_count)
    {
        printf("end of token-list reached\n");
        return NULL;
    }

    jet_token* cur_tok = jet_ast_peek_tok(ast);
    if(cur_tok->type == TOK_EOF)
    {
        jet_ast_consume_tok(ast);
        return NULL;
    }

    jet_ast_node* node = NULL;

    switch(cur_tok->type)
    {
        default:
        case TOK_INV:
            fprintf(stderr, "error: could not evaluate valid node.");
            return NULL;
        case TOK_EOF:
            printf("EOF token reached.\n");
            return NULL;
        case TOK_KWD_PROG:
            node = jet_ast_node_prog_parse(ast);
            break;
        case TOK_KWD_MEM:
            node = jet_ast_node_mem_parse(ast);
            break;
        case TOK_KWD_IF:
        case TOK_KWD_WHILE:
        case TOK_KWD_FOR:
        case TOK_KWD_RETURN:
            node = jet_ast_node_parse_ctrl_stmt(ast);
            break;
        case TOK_KWD_CHAR : 
        case TOK_KWD_VOID : 
        case TOK_KWD_INT  : 
        case TOK_KWD_FLOAT: 
        case TOK_KWD_STR  : 
        case TOK_KWD_BOOL : 
            //POTENTIAL: type_decl, func_decl, func_def
            node = jet_ast_node_tdecl_parse(ast);
            break;
        case TOK_KWD_NULL:
        case TOK_KWD_TRUE:
        case TOK_KWD_FALSE:
        case TOK_LIT_INT:
        case TOK_LIT_FLOAT:
        case TOK_LIT_CHAR:
        case TOK_LIT_STR:
            node = jet_ast_node_lit_parse(ast);
            break;
        case TOK_IDENT:
            //POTENTIAL: func_call, var_ref, expression_component
            //possibly lit in the case of constant ref
            node = jet_ast_node_ident_parse(ast);
            break; 
    }

    if(node == NULL)
    {
        fprintf(stderr, "error: could not generate AST node.\n");
    }
    return node;
}

static jet_ast_node* jet_ast_node_prog_parse(jet_ast* ast)
{
    jet_ast_expect_tok(ast, TOK_KWD_PROG);
    jet_ast_node* block = jet_ast_node_block_parse(ast);
    jet_ast_node* prog  = jet_astn_prog_create(block);
    return prog;
}

static jet_ast_node* jet_ast_node_mem_parse(jet_ast* ast)
{
    return NULL;
} 

static jet_ast_node* jet_ast_node_block_parse(jet_ast* ast)
{
    jet_list* node_list = jet_list_create(16, sizeof(jet_ast_node));
    assert(node_list != NULL);

    //TODO: populate node_list

    jet_ast_node* block = jet_astn_block_create(node_list);
    assert(block != NULL);
    return block;
}

static jet_ast_node* jet_ast_node_lit_parse(jet_ast* ast)
{
    jet_token* cur_tok = jet_ast_consume_tok(ast);
    jet_ast_node* lit = jet_astn_lit_create(cur_tok); 
    assert(lit != NULL);
    return lit;
}

static jet_ast_node* jet_ast_node_func_parse(jet_ast* ast)
{
    return NULL;
}

static jet_ast_node* jet_ast_node_tdecl_parse(jet_ast* ast)
{
    //add multi return support
    jet_token* tdecl_tok = jet_ast_consume_tok(ast);
    jet_ast_node* type_ident = jet_ast_node_tok_value_ident(tdecl_tok);
    if(!type_ident)
    {
        fprintf(stderr, "errro: type is not recognized, make sure all files are binded.\n");
        return NULL;
    }

    //declare
    //declared type_decl info
    size_t byte_size = jet_ast_get_type_byte_size(tdecl_tok->type);
    bool is_native = true;

    jet_ast_node* tdecl = jet_astn_tdecl_create(type_ident, byte_size, is_native);
    jet_ast_node* binding_ident = jet_ast_node_ident_parse(ast);
    jet_token* tok_after_ident = jet_ast_peek_tok(ast);

    jet_ast_node* vdecl = NULL;

    if(tok_after_ident == NULL)
    {
        fprintf(stderr, "error: incomplete type decl, expected one or more tokens after identifier.\n");
        return NULL;
    }
    switch(tok_after_ident->type)
    {
        default:
            fprintf(stderr, "error: expected ';', '=' or '(' after type-decleration.\n");
            return NULL;
        case TOK_SEMI:
            jet_ast_consume_tok(ast);
            vdecl = jet_astn_vdecl_create(binding_ident, tdecl, NULL);
            return vdecl;
        case TOK_ASG:
            jet_ast_consume_tok(ast);
            jet_ast_node* init_value = jet_ast_node_parse_expr(ast);
            vdecl = jet_astn_vdecl_create(binding_ident, tdecl, init_value);
            return vdecl;
        case TOK_LPAR:
            bool is_defined = false;
            jet_list* params_list = jet_ast_node_func_parse_params(ast, &is_defined);   

            jet_list* ret_type_list = jet_list_create(1, sizeof(jet_ast_node)); 
            jet_list_append(ret_type_list, tdecl);
            jet_ast_node* fdecl = jet_astn_fdecl_create(binding_ident, ret_type_list, params_list); 
            if(!is_defined)
            {
                return fdecl;
            }
            jet_ast_node* block = jet_ast_node_block_parse(ast);
            jet_ast_node* fdef = jet_astn_fdef_create(fdecl, block);
            return fdef;
    }
}

static jet_ast_node* jet_ast_node_ident_parse(jet_ast* ast)
{
    jet_token* ident_tok = jet_ast_expect_tok(ast, TOK_IDENT);
    jet_ast_node* ident = jet_astn_ident_create(ident_tok->source + ident_tok->origin, ident_tok->len);
    return ident;
}

static jet_ast_node* jet_ast_node_vdecl_parse(jet_ast* ast)
{
    return NULL;
}

static jet_ast_node* jet_ast_node_parse_ctrl_stmt(jet_ast* ast)
{
    return NULL;
}

static jet_ast_node* jet_ast_node_parse_expr(jet_ast* ast, size_t min_prec)
{
    /* schema
     *  - int i = TOK_IDENT  + 42
     *  - int i = TOK_LIT_INT + 42
     *  - int i = TOK_MINUS 42 + ident
     *  - int i = TOK_LPAR ident) + 42
     *  - bool b = TOK_NOT ident
     *  - bool b = TOK_KWD_TRUE
     *  - str s = TOK_LIT_STR
     *  - char c = TOK_LIT_CHAR
     *
     *  potential starts: 
     *      TOK_IDENT
     *      TOK_LIT_*
     *      TOK_MINUS
     *      TOK_NOT
     *      TOK_LPAR
     *      TOK_NOT
     *      TOK_KWD_TRUE
     *      TOK_KWD_FALSE
     *
     *  ending:
     *      TOK_SEMI
     * */

    jet_ast_node* lhs_node = jet_ast_node_parse_primary(ast);

    while(1)
    {
        jet_token* op_tok = jet_ast_peek_tok(ast);
        size_t op_prec = jet_ast_get_op_prec(op_tok->type);
        if(op_prec == 0)
            break;
        if(op_prec < min_prec)
            break;
        jet_ast_consume_tok(ast);
        
    }
}

static jet_ast_node* jet_ast_node_parse_primary(jet_ast* ast)
{
    
}

static jet_list* jet_ast_node_func_parse_params(jet_ast* ast, bool* out_defines_func)
{
    jet_list* list = jet_list_create(4, sizeof(jet_ast_node));
    assert(list != NULL);

    //TODO: populate params

    if(out_defines_func)
    {
        jet_token* tok = jet_ast_peek_tok(ast);
        if(tok != NULL && tok->type == TOK_RBRC)
        {
            *out_defines_func = true;
        }
    }
    return NULL;
}

static jet_ast_node* jet_ast_node_tok_value_ident(jet_token* tok)
{
    jet_ast_node* ident = jet_astn_ident_create(tok->source + tok->origin, tok->len);
    assert(ident != NULL);
    return ident;
}

static void jet_ast_add_top_node(jet_ast* ast, jet_ast_node* node)
{
    assert(ast != NULL);
    assert(node != NULL);
    assert(ast->top_node_list != NULL);
    jet_list_append(ast->top_node_list, (const void*)node);
}

static jet_token* jet_ast_expect_tok(jet_ast* ast, jet_token_type tok_type)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    if(ast->tok_cursor >= jet_list_count(ast->tok_list))
    {
        puts("end of token-list reached.");
        return NULL;
    }
    jet_token* tok = jet_ast_peek_tok(ast);
    assert(tok != NULL);
    assert(tok->type == tok_type);
    return jet_ast_consume_tok(ast);
}

static jet_token* jet_ast_peek_tok(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    if(jet_list_count(ast->tok_list) <= 0)
    {
        fprintf(stderr, "wrn: ast->tok_list is empty.\n");        
        return NULL; 
    }
    return (jet_token*)jet_list_get(ast->tok_list, ast->tok_cursor);
}

static jet_token* jet_ast_peek_next_tok(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);
    size_t tok_count = jet_list_count(ast->tok_list);
    if (tok_count <= 0)
    {
        fprintf(stderr, "wrn: ast->tok_list is empty.\n");        
        return NULL; 
    }
    if(ast->tok_cursor >= tok_count - 1)
    {
        puts("end of token-list reached.");
        return NULL;
    }
    return (jet_token*)jet_list_get(ast->tok_list, ast->tok_cursor + 1);
}

static jet_token* jet_ast_consume_tok(jet_ast* ast)
{
    assert(ast != NULL);
    assert(ast->tok_list != NULL);

    if(ast->tok_cursor >= jet_list_count(ast->tok_list))
    {
        puts("end of token-list reached.");
        return NULL;
    }
    ast->tok_cursor++;
    return (jet_token*)jet_list_get(ast->tok_list , ast->tok_cursor - 1);
}


static size_t jet_ast_get_type_byte_size(jet_token_type tok_type)
{
    switch(tok_type)
    {
        default:
            fprintf(stderr, "wrn: token type doesnt have built native identifier.\n");
            return 0;
        case TOK_KWD_VOID:
            return 0;
        case TOK_KWD_INT:
            return 4;
        case TOK_KWD_FLOAT:
            return 4;
        case TOK_KWD_BOOL:
            return 1;
        case TOK_KWD_CHAR:
            return 1;
        case TOK_KWD_STR:
            return 4;
    }
}    




















