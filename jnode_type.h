#pragma once

typedef enum jnode_type
{
  jnode_PROG,  
  jnode_IDENT,
  jnode_BLOCK,
  jnode_VAR_REF,
  jnode_VAR_DECL,
  jnode_TYPE_DECL,
  jnode_FUNC_DECL,
  jnode_FUNC_DEF,
  jnode_FUNC_CALL,
} jnode_type;
