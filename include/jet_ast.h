#pragma once
#include <jet_da.h>
#include <jet_token.h>
#include <jet_ast_node.h>

typedef struct jet_ast jet_ast;

jet_ast* jet_ast_create(jet_da* tok_darray);
bool jet_ast_dispose(jet_ast* ast);
bool jet_ast_generate_nodes(jet_ast* ast);
void jet_ast_print(jet_ast* ast);


