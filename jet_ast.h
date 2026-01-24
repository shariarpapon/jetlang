#pragma once
#include <jet_list.h>
#include <jet_token.h>
#include <jet_ast_node.h>

typedef struct jet_ast jet_ast;

jet_ast* jet_ast_create(jet_list* tok_list);
bool jet_ast_dispose(jet_ast* ast);
