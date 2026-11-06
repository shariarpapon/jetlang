#pragma once
#include <stddef.h>
#include <stdbool.h>

#define JET_CLI_FLAG_CMD ('-')
#define JET_CLI_FLAG_DELIM (',')

#define JET_CLI_MAX_FLAGS (256)

typedef struct jet_flag_registry jet_flag_registry;
typedef struct jet_flag jet_flag;
typedef struct jet_flag_arg jet_flag_arg;
typedef struct jet_flag_str jet_flag_str;

struct jet_flag_str
{
    const char* str;
    size_t len;
};

struct jet_flag_arg
{
    jet_flag_str name;
};

struct jet_flag
{
    jet_flag_str name;
    jet_flag_arg* args;
    size_t arg_count;
};

struct jet_flag_registry
{
    jet_flag flags[JET_CLI_MAX_FLAGS];
    size_t flag_count;
};

bool jet_flagreg_init(jet_flag_registry* fr, char** argv, size_t argc);
void jet_flagreg_dispose(jet_flag_registry* fr);

bool jet_flagreg_register_flag(jet_flag_registry* fr, jet_flag flag);
jet_flag* jet_flagreg_get_flag(jet_flag_registry* fr, const char* f);
bool jet_flagreg_has_flag(jet_flag_registry* fr, const char* f);

const jet_flag_arg* jet_flagreg_get_argi(jet_flag_registry* fr, const char* flag_name, size_t index);
const jet_flag_arg* jet_flagreg_get_arg(jet_flag_registry* fr, const char* flag, const char* arg);
bool jet_flagreg_has_arg(jet_flag_registry* fr, const char* f, const char* arg);























































































