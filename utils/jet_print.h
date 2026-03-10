#pragma once

#include <stdio.h>

#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_BLUE "\x1b[34m"

#define CPRINT(color, fmt, ...) \
    printf(color fmt "\x1b[0m", ##__VA_ARGS__)

