#pragma once

#include <stdio.h>

#define ANSI_PRINTF(color, fmt, ...) \
    printf(color fmt "\x1b[0m", ##__VA_ARGS__)

#define ANSI_RESET "\x1b[0m"
#define ANSI_RED1 "\x1b[31m"
#define ANSI_RED2 "\x1b[0;38;5;161;49m"
#define ANSI_BLUE1 "\x1b[34m"
#define ANSI_BLUE2 "\x1b[38;5;39m"
#define ANSI_ORANGE1 "\x1b[38;5;215m"
#define ANSI_GREEN1 "\x1b[32m"
#define ANSI_GREEN2 "\x1b[38;5;49m"
#define ANSI_GREY1 "\x1b[38;5;244m"
#define ANSI_PURPLE1 "\x1b[38;5;141m"
#define ANSI_PURPLE2 "\x1b[38;5;135m"
#define ANSI_PURPLE3 "\x1b[38;5;99m"
#define ANSI_CYAN1 "\x1b[38;5;51m"
#define ANSI_CYAN2 "\x1b[38;5;44m"
#define ANSI_CYAN3 "\x1b[38;5;37m"
#define ANSI_YELLOW1 "\x1b[38;5;226m"
#define ANSI_YELLOW2 "\x1b[38;5;220m"
#define ANSI_YELLOW3 "\x1b[38;5;178m"
#define ANSI_PINK1 "\x1b[38;5;213m"
#define ANSI_PINK2 "\x1b[38;5;206m"
#define ANSI_PINK3 "\x1b[38;5;199m"
#define ANSI_MAGENTA1 "\x1b[38;5;201m"
#define ANSI_MAGENTA2 "\x1b[38;5;165m"
#define ANSI_MAGENTA3 "\x1b[38;5;129m"
#define ANSI_TEAL1 "\x1b[38;5;80m"
#define ANSI_TEAL2 "\x1b[38;5;73m"
#define ANSI_TEAL3 "\x1b[38;5;66m"
#define ANSI_LIME1 "\x1b[38;5;118m"
#define ANSI_LIME2 "\x1b[38;5;112m"
#define ANSI_LIME3 "\x1b[38;5;76m"
#define ANSI_GOLD1 "\x1b[38;5;220m"
#define ANSI_GOLD2 "\x1b[38;5;214m"
#define ANSI_GOLD3 "\x1b[38;5;178m"
#define ANSI_BROWN1 "\x1b[38;5;130m"
#define ANSI_BROWN2 "\x1b[38;5;136m"
#define ANSI_BROWN3 "\x1b[38;5;94m"
#define ANSI_GREY2 "\x1b[38;5;245m"
#define ANSI_GREY3 "\x1b[38;5;240m"
#define ANSI_GREY4 "\x1b[38;5;236m"
#define ANSI_WHITE1 "\x1b[38;5;255m"
#define ANSI_WHITE2 "\x1b[38;5;252m"
#define ANSI_WHITE3 "\x1b[38;5;250m"






