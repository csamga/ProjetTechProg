#include "terminal.h"

#include <stdio.h>

#define CSI "\x1b["

void new_page(void) {
    clear_screen();
    set_cursor_home();
}

void clear_screen(void) {
    fputs(CSI "2J", stdout);
}

void use_screen_buffer(enum screen_buffer sb) {
    switch (sb) {
    case screen_buffer_alternative:
        fputs(CSI "?1049h", stdout);
        break;
    case screen_buffer_default:
        fputs(CSI "?1049l", stdout);
        break;
    }
}

void set_cursor_pos(short x, short y) {
    printf(CSI "%hd;%hdH", ++y, ++x);
}

void move_cursor_to_col(short x) {
    printf(CSI "%hdG", x);
}

void set_cursor_home(void) {
    fputs(CSI "H", stdout);
}

void save_cursor_pos(void) {
    fputs(CSI "s", stdout);
}

void restore_cursor_pos(void) {
    fputs(CSI "u", stdout);
}

void show_cursor(bool show) {
    if (show) {
        fputs(CSI "?25h", stdout);
    } else {
        fputs(CSI "?25l", stdout);
    }
}
