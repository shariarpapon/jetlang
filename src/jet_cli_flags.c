#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <jet_cli_flags.h>
static size_t jet_flagreg_max_arg_count(const char* input, char delim);
static bool jet_flagreg_parse_flag(const char* input, jet_flag* out, const char cmd_start, const char delim);

bool jet_flagreg_init(jet_flag_registry* fr, char** argv, size_t argc)
{
    if(fr == NULL) 
        return false;
    if(argv == NULL && argc > 0)
        return false;

    *fr = (jet_flag_registry){0};
    for(size_t i = 0; i < argc; i++)
    {
        jet_flag flag = (jet_flag){0};
        if(jet_flagreg_parse_flag(argv[i], &flag, JET_CLI_FLAG_CMD, JET_CLI_FLAG_DELIM))
        {
            if(!jet_flagreg_register_flag(fr, flag))
                return false;
        }
        else
            fprintf(stderr, "invalid argument '%s'\n", argv[i]);
    }
    return true;
}

void jet_flagreg_dispose(jet_flag_registry* fr)
{
    if(fr == NULL) return;
    for(size_t i = 0; i < fr->flag_count; i++)
        if(fr->flags[i].args)
            free((void*)fr->flags[i].args);
}

bool jet_flagreg_register_flag(jet_flag_registry* fr, jet_flag flag)
{
    if(fr == NULL || fr->flag_count == JET_CLI_MAX_FLAGS)  
        return false;
    fr->flags[fr->flag_count++] = flag;
    return true;
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
        if(flag.name.str == NULL || flag.name.len == 0) 
            continue;
        if(strlen(f) == flag.name.len && strncmp(flag.name.str, f, flag.name.len) == 0)
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
    for(size_t f = 0; f < fr->flag_count; f++)
    {
        jet_flag flag = fr->flags[f];
        if(flag.args == NULL || flag.arg_count == 0)
            continue;
        for(size_t i = 0; i < flag.arg_count; i++)
        {
            if(flag.args[i].name.str == NULL || flag.args[i].name.len != strlen(arg_str))
                continue;
            if(strncmp(flag.args[i].name.str, arg_str, flag.args[i].name.len) == 0)
                return &flag.args[i];
        }
    }

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

static size_t jet_flagreg_max_arg_count(const char* input, char delim)
{
    if(input == NULL) return 0;
    size_t count = 0;
    while(input && *input != '\0')
    {
        if(*input == delim) count++;
        input++;
    }
    return count;
}

//input = "-pr,ast" -> strlen=7
//input++
//input = "pr,ast" -> strlen=6
//len = 1 | *input = r
//len = 2 | *input = , -> flag_name{base, len} 
static bool jet_flagreg_parse_flag(const char* input, jet_flag* out, const char cmd_start, const char delim)
{
    if(input == NULL || out == NULL || 
       strlen(input) == 0 || *input != cmd_start) 
        return false;
    
    input++;
    if(*input == '\0') 
        return false;

    const char* start = input;
    size_t len = 0;
    
    *out = (jet_flag){0};
    size_t full_len = strlen(input) + 1;

    for(size_t i = 0; i < full_len; i++)
    {
        if(*input == delim || *input == '\0')
             break;
        len++;
        input++;
    }
    out->name = (jet_flag_str){.str = start, .len = len}; 

    if(*input == delim) 
        input++;
    else if(*input == '\0') 
        return true;  

    start = input;
    len = 0;

    size_t arg_count = 0;
    size_t max_arg_count = jet_flagreg_max_arg_count(input, delim);
    jet_flag_str arg_names[max_arg_count + 1];
    
    while(*input != '\0')
    {
        input++; 
        len++;
        if(*input == delim || *input == '\0')
        {
            jet_flag_str name = (jet_flag_str){.str = start, .len = len};
            arg_names[arg_count++] = name; 
            if(*input == '\0')
                break;
            else
            {
                while(*input == delim)
                    input++; 
            }
            
            len = 0;
            start = input;
            continue;
        }
    }

    if(arg_count == 0)
        return true;
    out->args = calloc(arg_count, sizeof(jet_flag_arg));
    if(out->args == NULL)
        return false;
    for(size_t i = 0; i < arg_count; i++)
        out->args[i] = (jet_flag_arg){.name = arg_names[i]};
    out->arg_count = arg_count;
    return true;
}

const jet_flag_arg* jet_flagreg_get_argi(jet_flag_registry* fr, const char* flag_name, size_t index)
{
    if(fr == NULL || flag_name == NULL) return NULL;
    
    for(size_t i = 0; i < fr->flag_count; i++)
    {
        jet_flag flag = fr->flags[i];
        if(strlen(flag_name) != flag.name.len) continue;
        if(strncmp(flag.name.str, flag_name, flag.name.len) == 0)
        {
            if(index >= flag.arg_count || flag.args == NULL) 
                continue;
            return &flag.args[index];
        }
    }
    return NULL;
}






