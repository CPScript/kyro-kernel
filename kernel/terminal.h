#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>
#include <stdbool.h>

#define TERMINAL_WIDTH 80
#define TERMINAL_HEIGHT 25
#define TERMINAL_BUFFER_SIZE 2048

typedef struct {
    char buffer[TERMINAL_BUFFER_SIZE];
    int buffer_index;
    int cursor_x;
    int cursor_y;
    uint8_t color;
    bool echo_enabled;
} terminal_t;

extern terminal_t main_terminal;

void terminal_init(void);
void terminal_clear(void);
void terminal_putchar(char c);
void terminal_puts(const char *str);
void terminal_set_color(uint8_t fg, uint8_t bg);
void terminal_move_cursor(int x, int y);
void terminal_scroll_up(void);
char terminal_getchar(void);
void terminal_get_line(char *buffer, int max_length);

// Terminal colors
#define TERMINAL_COLOR_BLACK     0
#define TERMINAL_COLOR_BLUE      1
#define TERMINAL_COLOR_GREEN     2
#define TERMINAL_COLOR_CYAN      3
#define TERMINAL_COLOR_RED       4
#define TERMINAL_COLOR_MAGENTA   5
#define TERMINAL_COLOR_BROWN     6
#define TERMINAL_COLOR_LGRAY     7
#define TERMINAL_COLOR_DGRAY     8
#define TERMINAL_COLOR_LBLUE     9
#define TERMINAL_COLOR_LGREEN    10
#define TERMINAL_COLOR_LCYAN     11
#define TERMINAL_COLOR_LRED      12
#define TERMINAL_COLOR_LMAGENTA  13
#define TERMINAL_COLOR_YELLOW    14
#define TERMINAL_COLOR_WHITE     15

#endif
