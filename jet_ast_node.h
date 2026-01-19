#pragma once
#include <jet_list.h>
#include <jnode_type.h>

// PROGRAM ENTRY POINT`:vs 
typedef struct jnode_prog 
{
  jnode* block;
} jnode_prog;

typedef struct jnode_ident
{
  const char* value;
} jnode_ident;
 
 typedef struct jnode_lit
 {
     /*
        -1       0       1       2       3       4
        void    int     float   bool    char    str
     */
     int lit_type;
     union
     {
         void* n;
         int i;
         float f;
         bool b;
         char c;
         const char* s;
     } as;
 } jnode_lit;

typedef struct jnode_block
{
  jet_list* nodes;
} jnode_block;

typedef struct jnode_var_ref 
{
  jnode* var_binding_ident;
} jnode_var_ref;

typedef struct jnode_var_decl
{  
  jnode* binding_ident;
  jnode* type_decl;
  jnode* init_value;
} jnode_var_decl;

typedef struct jnode_type_decl 
{
  jnode* type_ident;
  size_t byte_size;
  bool is_native;
} jnode_type_decl;

typedef struct jnode_func_decl
{
  jnode* binding_ident;
  jet_list* ret_type_decls;
  jet_list* param_var_decls; 
} jnode_func_decl;

typedef struct jnode_func_def
{
  jnode* func_decl;
  jnode* block;
} jnode_func_def; 

typedef struct jnode_func_call
{
  jnode* func_binding_ident;
} jnode_func_call;

// NODE BASE STRUCT
typedef struct jnode
{
    jnode_type node_type;
    union
    {
        jnode_prog* prog;
        jnode_ident* ident;
        jnode_lit* lit;
        jnode_block* block; 
        jnode_var_ref* var_ref;
        jnode_var_decl* var_decl;
        jnode_type_decl* type_decl;
        jnode_func_decl* func_decl;
        jnode_func_def* func_def;
        jnode_func_call* func_call;
    } as;
} jnode;







