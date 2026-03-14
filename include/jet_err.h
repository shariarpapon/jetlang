#pragma once
#include <jet_token.h>
#include <jet_ast.h>
#include <jet_span.h>

typedef enum jet_err_type jet_err_type;
typedef enum jet_err_level jet_err_level;
typedef struct jet_err jet_err;

enum jet_err_type
{
    JET_ERR_LEXER,
    JET_ERR_PARSER,
    JET_ERR_SEMANTIC,
};

enum jet_err_level
{
    JET_ERR_FATAL,
    JET_ERR_ERROR,
    JET_ERR_WARNING,
    JET_ERR_INFO,
};

struct jet_err
{
    jet_err_type type;
    jet_err_level level;
    jet_span span;
    const char* msg;
};

