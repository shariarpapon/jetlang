#pragma once
#include <jet_list.h>
#include <jnode_type.h>

typedef struct jnode
{
    jnode_type node_type;
    union
    {
        jnode_prog* prog;
        jnode_block* block;
        jnode_type_decl* type_decl;
        jnode_func_def* func_def;
    }
} jnode;

// PROGRAM ENTRY POINT
typedef struct jnode_prog 
{
  jet_list* nodes;
} jnode_prog;

typedef struct jnode_block
{
  jet_list* nodes;
} jnode_block;

typedef struct jnode_type_decl 
{
  const char* binding_name;
  size_t byte_size;
  jnode* init_value;
} jnode_type_decl;

typedef struct jnode_func_def
{
  const char* binding_name;
  jet_list* param_nodes; 
  jnode* block;
} jnode_func_def; 








