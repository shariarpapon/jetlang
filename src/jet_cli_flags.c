#include <string.h>
#include <jet_cli_flags.h>

bool jet_flagreg_init(jet_flag_registry* fr, char** argv, size_t argc)
{
    if(fr == NULL) 
        return false;
    if(argv == NULL && argc > 0)
        return false;

    char** flags = argv;
    for(size_t i = 0; i < argc; i++)
    {
        char* arg = *flags;
        flags++;
    }
}
jet_flag* jet_flagreg_get_flag(jet_flag_registry* fr, const char* f)
{
    if(fr == NULL || f == NULL 
       || fr->flags == NULL 
       || fr->flag_count == 0)
        return NULL;

    for(size_t i = 0; i < fr->flag_count; i++)
    {
        jet_flag flag = fr->flags[i];
        if(flag.str == NULL) 
            continue;
        if(strcmp(flag.str, f) == 0)
            return &fr->flags[i];
    }
    return NULL;
}

bool jet_flagreg_has_flag(jet_flag_registry* fr, const char* f)
{
    if(fr == NULL || f == NULL 
       || fr->flags == NULL 
       || fr->flag_count == 0)
        return false;
    return jet_flagreg_get_flag(fr, f) != NULL;
}

const jet_flag_arg* jet_flagreg_get_arg(jet_flag_registry* fr, const char* flag_str, const char* arg_str)
{
    jet_flag* flag = jet_flagreg_get_flag(fr, flag_str);
    if(flag == NULL) 
        return NULL;
    if(flag->args == NULL || flag->arg_count == 0)
        return NULL;
    for(size_t i = 0; i < flag->arg_count; i++)
        if(strcmp(flag->args[i].str, arg_str) == 0)
            return &flag->args[i];
    return NULL;
}

bool jet_flagreg_has_arg(jet_flag_registry* fr, const char* f, const char* a)
{
    if(fr == NULL || f == NULL || a == NULL)
        return false;
    if(fr->flags == NULL)
        return false;
    const jet_flag_arg* arg = jet_flagreg_get_arg(fr ,f, a);
    return arg != NULL;
}






