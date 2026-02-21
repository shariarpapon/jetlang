#include <jet_ast_node_create.h>
#include <jet_token.h>
#include <jet_conv.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

jet_ast_node* 
   jet_astn_prog_create(jet_ast_node* block) 
{  
    assert(block != NULL);
    jet_ast_node_prog* prog = (jet_ast_node_prog*)malloc(sizeof(jet_ast_node_prog));
    assert(prog != NULL);
    prog->block = block;
    jet_ast_node* node = jet_ast_node_create_base(AST_PROG);
    assert(node != NULL);
    node->as.prog = prog;
    return node;
}

jet_ast_node* jet_astn_mem_create(size_t alloc_size)
{
    if(alloc_size == 0)
    {

        fprintf(stderr, "error: attempting to allocate zero bytes.\n");
        return NULL;
    }
    jet_ast_node_mem* mem = (jet_ast_node_mem*)malloc(sizeof(jet_ast_node_mem));
    assert(mem != NULL);
    mem->alloc_size = alloc_size;
    jet_ast_node* node = jet_ast_node_create_base(AST_MEM);
    assert(node != NULL);
    node->as.mem = mem;
    return node;
}

jet_ast_node* 
   jet_astn_block_create(jet_list* node_list) 
{ 
    assert(node_list != NULL);
    jet_ast_node_block* block = (jet_ast_node_block*)malloc(sizeof(jet_ast_node_block));
    assert(block != NULL);    
    block->node_list = node_list;
    
    jet_ast_node* node = jet_ast_node_create_base(AST_BLOCK);
    assert(node != NULL);
    node->as.block = block;
    return node;
}

jet_ast_node* 
   jet_astn_ident_create(const char* str)
{
    jet_ast_node_ident* ident = (jet_ast_node_ident*)malloc(sizeof(jet_ast_node_ident));
    assert(ident != NULL);
    ident->str = str; 
    jet_ast_node* node = jet_ast_node_create_base(AST_IDENT);
    assert(node != NULL);
    node->as.ident = ident;
    return node;
}


jet_ast_node* 
   jet_astn_lit_create(jet_token* tok) 
{
    assert(tok != NULL);

    jet_ast_node_lit* lit = (jet_ast_node_lit*)malloc(sizeof(jet_ast_node_lit));
    assert(lit != NULL);
    
    switch(tok->type)
    {
        default: 
        {
            fprintf(stderr, "error: token type not recognized as literal.\n");
            return NULL;
        }
        case TOK_KWD_NULL:  
        {    
            lit->as.v = NULL; 
            break;
        }
        case TOK_LIT_INT:   
        {    
            lit->as.i = jet_conv_stoi(tok->source + tok->origin, tok->len); 
            break; 
        }
        case TOK_LIT_FLOAT: 
        {
            lit->as.f = jet_conv_stof(tok->source + tok->origin, tok->len);  
            break; 
        }
        case TOK_KWD_TRUE:  
        {
            lit->as.b = true; 
            break;
        }
        case TOK_KWD_FALSE: 
        {
            lit->as.b = false; 
            break;
        }
        case TOK_LIT_CHAR:  
        {
            lit->as.c = *(tok->source + tok->origin); 
            break; 
        }
        case TOK_LIT_STR:
        {
                 char* sub = (char*)malloc((tok->len + 1) * sizeof(char));
                 assert(sub != NULL);
                 memcpy(sub, tok->source + tok->origin, tok->len);
                 sub[tok->len] = '\0';
                 lit->as.s = sub;
                 break;
        }
    }
    lit->lit_type = tok->type;
    jet_ast_node* node = jet_ast_node_create_base(AST_LIT);
    assert(node != NULL);
    node->as.lit = lit;
    return node;
}

jet_ast_node* 
   jet_astn_vdecl_create(
      jet_ast_node* type_decl, 
      jet_ast_node* ident, 
      jet_ast_node* init_value) 
{
    assert(ident != NULL && type_decl != NULL);
    jet_ast_node_var_decl* vdecl = 
        (jet_ast_node_var_decl*)malloc(sizeof(jet_ast_node_var_decl));
    assert(vdecl != NULL);
    vdecl->ident = ident;
    vdecl->type_decl = type_decl;
    vdecl->init_value = init_value;
    
    jet_ast_node* node = jet_ast_node_create_base(AST_VAR_DECL);
    assert(node != NULL);
    node->as.var_decl = vdecl;
    return node;
}

jet_ast_node*
   jet_astn_tdecl_create(
      const char* type_name,
      size_t byte_size, 
      bool is_native) 
{ 
    assert(type_name != NULL);
    jet_ast_node_type_decl* tdecl = 
        (jet_ast_node_type_decl*)malloc(sizeof(jet_ast_node_type_decl));
    assert(tdecl != NULL);
    
    tdecl->type_name = type_name;
    tdecl->byte_size = byte_size;
    
    jet_ast_node* node = jet_ast_node_create_base(AST_TYPE_DECL);
    assert(node != NULL);
    node->as.type_decl = tdecl;
    return node;
}

jet_ast_node* 
   jet_astn_fdecl_create(
      jet_ast_node* ident,
      jet_list* ret_type_list,
      jet_list* param_list) 
{
    jet_ast_node_func_decl* fdecl = 
        (jet_ast_node_func_decl*)malloc(sizeof(jet_ast_node_func_decl));
    assert(fdecl != NULL);
    
    fdecl->ident = ident;
    fdecl->ret_type_list = ret_type_list;
    fdecl->param_list = param_list;
    
    jet_ast_node* node = jet_ast_node_create_base(AST_FUNC_DECL);
    assert(node != NULL);
    node->as.func_decl = fdecl;
    return node;
}

jet_ast_node* 
   jet_astn_fdef_create(
      jet_ast_node* func_decl, 
      jet_ast_node* block) 
{ 
    assert(func_decl != NULL && block != NULL);
    jet_ast_node_func_def* fdef = 
        (jet_ast_node_func_def*)malloc(sizeof(jet_ast_node_func_def));
    assert(fdef != NULL);
    
    fdef->func_decl = func_decl;
    fdef->block = block;
    
    jet_ast_node* node = jet_ast_node_create_base(AST_FUNC_DEF);
    assert(node != NULL);
    node->as.func_def = fdef;
    return node;
}

jet_ast_node* 
   jet_astn_call_create(
      jet_ast_node* ident, 
      jet_list* arg_list) 
{ 
    assert(ident != NULL && arg_list != NULL);
    jet_ast_node_call* fcall = 
        (jet_ast_node_call*)malloc(sizeof(jet_ast_node_call));
    assert(fcall != NULL);
     
    fcall->ident = ident;
    fcall->arg_list = arg_list;

    jet_ast_node* node = jet_ast_node_create_base(AST_CALL);
    assert(node != NULL);
    node->as.call = fcall;
    return node;
}


jet_ast_node* 
    jet_astn_binop_create(
       jet_ast_node* lhs, 
       jet_ast_node* rhs, 
       jet_token_type op_type)
{
    assert(lhs != NULL);
    assert(rhs != NULL);

    jet_ast_node_binop* binop = 
        (jet_ast_node_binop*)malloc(sizeof(jet_ast_node_binop));
    assert(binop != NULL);
    
    binop->lhs = lhs;
    binop->rhs = rhs;
    binop->op_type = op_type;

    jet_ast_node* node = jet_ast_node_create_base(AST_BINOP);
    assert(node != NULL);
    
    node->as.binop = binop;
    return node;
}


jet_ast_node* 
    jet_astn_unop_create(
        jet_ast_node* term, 
        jet_token_type op_type)
{
    assert(term != NULL);
    jet_ast_node_unop* unop = (jet_ast_node_unop*)malloc(sizeof(jet_ast_node_unop));
    assert(unop != NULL);
    
    unop->term = term;
    unop->op_type = op_type;
    
    jet_ast_node* node = jet_ast_node_create_base(AST_UNOP);
    assert(node != NULL);
    
    node->as.unop = unop;
    return node;
}

