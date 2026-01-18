#pragma once
#include <jet_list.h>

typedef enum jnode_type
{
  jnode_FUNC_DECL,
  jnode_FUNC_DEF,
  jnode_VAR_DECL,
} jnode_type;

typedef struct jnode
{
    jnode_type node_type;
    union
    {
        jnode_func_decl* func_decl;
        jnode_func_def* func_def;
        jnode_var_decl* var_decl;
    }
} jnode;









