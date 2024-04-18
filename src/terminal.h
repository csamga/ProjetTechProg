#ifndef TERMINAL_H_INCLUDED
#define TERMINAL_H_INCLUDED

#include <stdbool.h>
#include <stdio.h>

#define CSI "\x1b["
#define BOLD CSI "1m"
#define NORMAL CSI "0m"
#define BLUE CSI "94m"

enum screen_buffer {
    screen_buffer_default,
    screen_buffer_alternative
};

void new_page(void);

void log_title(const char *format, ...);
void log_success(const char *format, ...);
void log_info(bool confirm, const char *format, ...);
void log_warning(bool confirm, const char *format, ...);
void log_error(const char *format, ...);

void clear_screen(void);
void use_screen_buffer(enum screen_buffer sb, FILE *file);
void set_cursor_pos(short x, short y);
void move_cursor_to_col(short x);
void set_cursor_home(void);
void save_cursor_pos(void);
void restore_cursor_pos(void);
void show_cursor(bool show);
void erase_line(void);
void move_cursor_up(short n_lines);

#endif
