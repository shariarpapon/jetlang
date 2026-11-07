#pragma once
#include <stdbool.h>
#include <jet_ast_node.h>
#include <jet_error.h>

typedef struct jet_result jet_result;
typedef enum jet_result_kind jet_result_kind;

enum jet_result_kind 
{
    JET_RESULT_ERROR = 0,
    JET_RESULT_NID,
};

struct jet_result
{
    jet_result_kind kind;
    union
    {
        node_id nid;
        jet_error error;
    } as;
};

jet_result jet_result_nid(node_id nid);
jet_result jet_result_error(jet_error* error);




