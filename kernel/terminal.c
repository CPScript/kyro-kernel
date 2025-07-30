#include "terminal.h"
#include "kernel.h"

terminal_t main_terminal;

void terminal_init(void) {
    main_terminal.buffer_index = 0;
    main_terminal.cursor_x = 0;
    main_terminal.cursor_y = 0;
    main_terminal.color = 0x07;
    main_terminal.echo_enabled = true;
    print_message("Terminal initialized.\n");
}

void terminal_clear(void) {
    clear_screen();
}

void terminal_putchar(char c) {
    putchar(c);
}

void terminal_puts(const char *str) {
    while (*str) {
        terminal_putchar(*str++);
    }
}

void terminal_set_color(uint8_t fg, uint8_t bg) {
    main_terminal.color = (bg << 4) | (fg & 0x0F);
}

void terminal_move_cursor(int x, int y) {
    main_terminal.cursor_x = x;
    main_terminal.cursor_y = y;
}

void terminal_scroll_up(void) {
    // Scrolling handled in kernel.c print_message
}

char terminal_getchar(void) {
    return keyboard_get_char();
}

void terminal_get_line(char *buffer, int max_length) {
    int pos = 0;
    char c;
    
    while (pos < max_length - 1) {
        while ((c = terminal_getchar()) == 0) {
            asm volatile("hlt");
        }
        
        if (c == '\n' || c == '\r') {
            buffer[pos] = '\0';
            terminal_putchar('\n');
            return;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                terminal_putchar('\b');
                terminal_putchar(' ');
                terminal_putchar('\b');
            }
        } else if (c >= 32 && c <= 126) {
            buffer[pos++] = c;
            terminal_putchar(c);
        }
    }
    buffer[pos] = '\0';
}