#pragma once

#include <stdio.h>

#define COLOR_RED "\x1b[31m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_ORANGE "\033[38;5;215m"
#define COLOR_GREEN1 "\x1b[32m"
#define COLOR_GREEN2 "\033[38;5;49m"
#define COLOR_GREY "\033[38;5;244m"

#define CPRINT(color, fmt, ...) \
    printf(color fmt "\x1b[0m", ##__VA_ARGS__)

