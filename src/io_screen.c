#include "gwbasic.h"

void io_init_screen(void) {
    // Screen initialization
}

void io_close_screen(void) {
    // Screen cleanup
}

void io_cls(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void io_locate(int row, int col) {
    if (row < 1) row = 1;
    if (col < 1) col = 1;
    printf("\033[%d;%dH", row, col);
    fflush(stdout);
}

void io_color(int fg, int bg) {
    int ansi_fg = 30 + (fg % 8);
    int ansi_bg = 40 + (bg % 8);
    printf("\033[%d;%dm", ansi_fg, ansi_bg);
    fflush(stdout);
}
