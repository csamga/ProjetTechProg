#include "terminal.h"
#include "input.h"

#include <stdio.h>
#include <stdarg.h>

enum text_color {
    normal = 39,
    red = 91,
    green = 92,
    yellow = 93,
    blue = 94
};

static void set_text_attr(enum text_color color, bool bold, FILE *file);
static void reset_text_attr(FILE *file);

void new_page(void) {
    clear_screen();
    set_cursor_home();
    fflush(stdout);
}

void log_title(const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(normal, true, stdout);
    vprintf(format, args);
    reset_text_attr(stdout);

    puts("");
}

void log_success(const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(green, true, stdout);
    vprintf(format, args);
    reset_text_attr(stdout);

    if (getchar() != '\n') {
        input_flush_stdin();
    }
}

void log_info(bool confirm, const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(blue, true, stdout);
    vprintf(format, args);
    reset_text_attr(stdout);

    if (confirm && (getchar() != '\n')) {
        input_flush_stdin();
    }
}

void log_warning(bool confirm, const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(yellow, true, stdout);
    vprintf(format, args);
    reset_text_attr(stdout);

    if (confirm && (getchar() != '\n')) {
        input_flush_stdin();
    }
}

void log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);

    set_text_attr(red, true, stderr);
    vfprintf(stderr, format, args);
    reset_text_attr(stderr);

    if (getchar() != '\n') {
        input_flush_stdin();
    }
}

void clear_screen(void) {
    fputs(CSI "2J", stdout);
}

void use_screen_buffer(enum screen_buffer sb, FILE *file) {
    switch (sb) {
    case screen_buffer_alternative:
        fputs(CSI "?1049h", file);
        break;
    case screen_buffer_default:
        fputs(CSI "?1049l", file);
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

static void set_text_attr(enum text_color color, bool bold, FILE *file) {
    fprintf(file, CSI "%dm" CSI "%dm", bold, color);
}

static void reset_text_attr(FILE *file) {
    fputs(CSI "0m", file);
}
