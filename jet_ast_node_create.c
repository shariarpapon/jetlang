#include <jet_ast_node_create.h>
#include <jet_token.h>
#include <jet_conv.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

jet_ast_node_prog* 
   jet_astn_prog_create(jet_ast_node* block) 
{  
    assert(block != NULL);
    jet_ast_node_prog* prog = (jet_ast_node_prog*)malloc(sizeof(jet_ast_node_prog));
    assert(prog != NULL);
    prog->block = block;
    return prog;
}

jet_ast_node_block* 
   jet_astn_block_create(jet_list* node_list) 
{ 
    assert(node_list != NULL);
    jet_ast_node_block* block = (jet_ast_node_block*)malloc(sizeof(jet_ast_node_block));
    assert(block != NULL);    
    block->node_list = node_list;
    return block;
}

jet_ast_node_ident* 
   jet_astn_ident_create(const char* str)
{
    assert(str != NULL);
    jet_ast_node_ident* ident = (jet_ast_node_ident*)malloc(sizeof(jet_ast_node_ident));
    assert(ident != NULL);
    ident->str = str;
    return ident;
}


jet_ast_node_lit* 
   jet_astn_lit_create(jet_token* tok) 
{
    assert(tok != NULL);

    jet_ast_node_lit* lit = (jet_ast_node_lit*)malloc(sizeof(jet_ast_node_lit));
    assert(lit != NULL);
    
    switch(tok->type)
    {
        default: lit->as.v            = NULL; break;
        case TOK_INT_LIT:   lit->as.i = jet_conv_stoi(tok->source + tok->origin, tok->len); break; 
        case TOK_FLOAT_LIT: lit->as.f = jet_conv_stof(tok->source + tok->origin, tok->len);  break; 
        case TOK_KWD_TRUE:  lit->as.b = true; break;
        case TOK_KWD_FALSE: lit->as.b = false; break;
        case TOK_CHAR_LIT:  lit->as.c = *(tok->source + tok->origin); break; 
        case TOK_STR_LIT:
                 char* sub = (char*)malloc((tok->len + 1) * sizeof(char));
                 assert(sub != NULL);
                 memcpy(sub, tok->source + tok->origin, tok->len);
                 sub[tok->len] = '\0';
                 break;
    }
    return lit;
}


jet_ast_node_var_ref* 
   jet_astn_vref_create(jet_ast_node* var_binding_ident) 
{
    assert(var_binding_ident != NULL);
    jet_ast_node_var_ref* vref = 
        (jet_ast_node_var_ref*)malloc(sizeof(jet_ast_node_var_ref));
    assert(vref != NULL);
    vref -> var_binding_ident = var_binding_ident;
    return vref;
}

jet_ast_node_var_decl* 
   jet_astn_vdecl_create(
      jet_ast_node* binding_ident, 
      jet_ast_node* type_decl, 
      jet_ast_node* init_value) 
{
    assert(binding_ident != NULL && type_decl != NULL && init_value != NULL);
    jet_ast_node_var_decl* vdecl = 
        (jet_ast_node_var_decl*)malloc(sizeof(jet_ast_node_var_decl));
    assert(vdecl != NULL);
    vdecl->binding_ident = binding_ident;
    vdecl->type_decl = type_decl;
    vdecl->init_value = init_value;
    return vdecl;
}

jet_ast_node_type_decl*
   jet_astn_tdecl_create(
      jet_ast_node* type_ident, 
      size_t byte_size, 
      bool is_native) 
{ 
    assert(type_ident != NULL);
    jet_ast_node_type_decl* tdecl = 
        (jet_ast_node_type_decl*)malloc(sizeof(jet_ast_node_type_decl));
    assert(tdecl != NULL);
    tdecl->type_ident = type_ident;
    tdecl->byte_size = byte_size;
    return tdecl;
}

jet_ast_node_func_decl* 
   jet_astn_fdecl_create(
      jet_ast_node* binding_ident,
      jet_list* ret_type_decls,
      jet_list* param_var_decls) 
{
    jet_ast_node_func_decl* fdecl = 
        (jet_ast_node_func_decl*)malloc(sizeof(jet_ast_node_func_decl));
    assert(fdecl != NULL);
    fdecl->binding_ident = binding_ident;
    fdecl->ret_type_decls = ret_type_decls;
    fdecl->param_var_decls = param_var_decls;
    return fdecl;
}

jet_ast_node_func_def* 
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
    return fdef;
}

jet_ast_node_func_call* 
   jet_astn_fcall_create(
      jet_ast_node* func_binding_ident, 
      jet_list* arg_nodes) 
{ 
    assert(func_binding_ident != NULL && arg_nodes != NULL);
    jet_ast_node_func_call* fcall = 
        (jet_ast_node_func_call*)malloc(sizeof(jet_ast_node_func_call));
    assert(fcall != NULL);
    return fcall;
}




