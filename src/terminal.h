#ifndef TERMINAL_H_INCLUDED
#define TERMINAL_H_INCLUDED

#include <stdbool.h>

#define CSI "\x1b["
#define BOLD CSI "1m"
#define NORMAL CSI "0m"

enum screen_buffer {
    screen_buffer_default,
    screen_buffer_alternative
};

enum text_color {
    normal = 39,
    red = 91,
    green = 92,
    yellow = 93,
    blue = 94
};

void new_page(void);

void clear_screen(void);
void use_screen_buffer(enum screen_buffer sb);
void set_cursor_pos(short x, short y);
void move_cursor_to_col(short x);
void set_cursor_home(void);
void save_cursor_pos(void);
void restore_cursor_pos(void);
void show_cursor(bool show);
void erase_line(void);
void move_cursor_up(short n_lines);

void set_text_attr(enum text_color color, bool bold);
void reset_text_attr(void);

void log_title(const char *format, ...);
void log_success(const char *format, ...);
void log_info(const char *format, ...);
void log_warning(const char *format, ...);
void log_error(const char *format, ...);

#endif
