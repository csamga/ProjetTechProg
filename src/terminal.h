#ifndef TERMINAL_H_INCLUDED
#define TERMINAL_H_INCLUDED

#include <stdbool.h>

enum screen_buffer {
    screen_buffer_default,
    screen_buffer_alternative
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

#endif
