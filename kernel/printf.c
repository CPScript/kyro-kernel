#include <stdarg.h>
#include <stdint.h>
#include "kernel.h"

static char *video_memory = (char *)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;

void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2] = ' ';
        }
    } else {
        video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2] = c;
        video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2 + 1] = 0x07; // White on black
        cursor_x++;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= VGA_HEIGHT) {
        // Scroll screen up
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            video_memory[i * 2] = video_memory[(i + VGA_WIDTH) * 2];
            video_memory[i * 2 + 1] = video_memory[(i + VGA_WIDTH) * 2 + 1];
        }
        
        // Clear last line
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            video_memory[i * 2] = ' ';
            video_memory[i * 2 + 1] = 0x07;
        }
        
        cursor_y = VGA_HEIGHT - 1;
    }
}

void puts(const char *str) {
    while (*str) {
        putchar(*str++);
    }
}

static void print_number(unsigned int num, int base) {
    char digits[] = "0123456789ABCDEF";
    char buffer[32];
    int i = 0;
    
    if (num == 0) {
        putchar('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = digits[num % base];
        num /= base;
    }
    
    while (--i >= 0) {
        putchar(buffer[i]);
    }
}

int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    int count = 0;
    
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                case 'i': {
                    int num = va_arg(args, int);
                    if (num < 0) {
                        putchar('-');
                        num = -num;
                        count++;
                    }
                    print_number(num, 10);
                    count++;
                    break;
                }
                case 'u':
                    print_number(va_arg(args, unsigned int), 10);
                    count++;
                    break;
                case 'x':
                    print_number(va_arg(args, unsigned int), 16);
                    count++;
                    break;
                case 'X':
                    print_number(va_arg(args, unsigned int), 16);
                    count++;
                    break;
                case 'c':
                    putchar(va_arg(args, int));
                    count++;
                    break;
                case 's': {
                    char *str = va_arg(args, char*);
                    if (str) {
                        puts(str);
                        count++;
                    }
                    break;
                }
                case '%':
                    putchar('%');
                    count++;
                    break;
                default:
                    putchar('%');
                    putchar(*format);
                    count += 2;
                    break;
            }
        } else {
            putchar(*format);
            count++;
        }
        format++;
    }
    
    va_end(args);
    return count;
}

int scanf(const char *format, ...) {
    // Basic stub - keyboard input not implemented yet
    return 0;
}

void print_message(const char *message) {
    puts(message);
}

void clear_screen(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x07;
    }
    cursor_x = 0;
    cursor_y = 0;
}
