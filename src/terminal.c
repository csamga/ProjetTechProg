#include "terminal.h"

#include <stdio.h>
#include <stdarg.h>

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

void erase_line(void) {
    fputs(CSI "2K", stdout);
}

void move_cursor_up(short n_lines) {
    printf(CSI "%hdF", n_lines);
}

void set_text_attr(enum text_color color, bool bold) {
    printf(CSI "%dm" CSI "%dm", bold, color);
}

void reset_text_attr(void) {
    fputs(CSI "0m", stdout);
}

void log_title(const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(normal, true);
    vprintf(format, args);
    reset_text_attr();
}

void log_success(const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(green, true);
    vprintf(format, args);
    reset_text_attr();
    getchar();
}

void log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(blue, true);
    vprintf(format, args);
    reset_text_attr();
    getchar();
}

void log_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(yellow, true);
    vprintf(format, args);
    reset_text_attr();
    getchar();
}

void log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(red, true);
    vfprintf(stderr, format, args);
    reset_text_attr();
    getchar();
}
