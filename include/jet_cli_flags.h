#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef struct jet_flag_registry jet_flag_registry;
typedef struct jet_flag jet_flag;
typedef struct jet_flag_arg jet_flag_arg;

struct jet_flag_registry
{
    jet_flag* flags;
    size_t flag_count;
};

struct jet_flag
{
    const char* str;
    const jet_flag_arg* args;
    size_t arg_count;
};

struct jet_flag_arg
{
    const char* str;
};

bool jet_flagreg_init(jet_flag_registry* fr, char** argv, size_t argc);
jet_flag* jet_flagreg_get_flag(jet_flag_registry* fr, const char* f);
bool jet_flagreg_has_flag(jet_flag_registry* fr, const char* f);

const jet_flag_arg* jet_flagreg_get_arg(jet_flag_registry* fr, const char* flag, const char* arg);
bool jet_flagreg_has_arg(jet_flag_registry* fr, const char* f, const char* arg);























































































