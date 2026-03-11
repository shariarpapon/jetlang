#pragma once

#include <stdio.h>

#define JETC_RED "\x1b[31m"
#define JETC_BLUE "\x1b[34m"
#define JETC_ORANGE "\033[38;5;215m"
#define JETC_GREEN1 "\x1b[32m"
#define JETC_GREEN2 "\033[38;5;49m"
#define JETC_GREY "\033[38;5;244m"

#define JET_CPRINT(color, fmt, ...) \
    printf(color fmt "\x1b[0m", ##__VA_ARGS__)

