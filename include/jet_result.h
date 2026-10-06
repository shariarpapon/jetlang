#pragma once
#include <stdbool.h>
#include <jet_ast_node.h>
#include <jet_error.h>

typedef struct jet_result
{
    bool success;
    union
    {
        node_id nid;
        jet_error error;
    } as;
} jet_result;

jet_result jet_result_nid(node_id nid);
jet_result jet_result_error(jet_error* error);




