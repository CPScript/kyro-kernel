#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "kernel.h"
#include "drivers/keyboard.h"

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

// Basic keyboard input function
char getchar_blocking(void) {
    char c = 0;
    while (c == 0) {
        if (keyboard_data_available()) {
            c = keyboard_get_char();
            if (c != 0) {
                // Echo the character
                if (c == '\b') {
                    putchar('\b');
                } else if (c == '\n' || c == '\r') {
                    putchar('\n');
                } else if (c >= 32 && c <= 126) {
                    putchar(c);
                }
                return c;
            }
        }
        // Give CPU a break
        asm volatile("hlt");
    }
    return c;
}

// Simple string input function
void gets_simple(char *buffer, int max_len) {
    int pos = 0;
    char c;
    
    while (pos < max_len - 1) {
        c = getchar_blocking();
        
        if (c == '\n' || c == '\r') {
            buffer[pos] = '\0';
            return;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
        } else if (c >= 32 && c <= 126) {
            buffer[pos++] = c;
        }
    }
    buffer[pos] = '\0';
}

static void print_number_to_buffer(char **buffer, unsigned int num, int base) {
    char digits[] = "0123456789ABCDEF";
    char temp[32];
    int i = 0;
    
    if (num == 0) {
        **buffer = '0';
        (*buffer)++;
        return;
    }
    
    while (num > 0) {
        temp[i++] = digits[num % base];
        num /= base;
    }
    
    while (--i >= 0) {
        **buffer = temp[i];
        (*buffer)++;
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

int vsprintf(char *str, const char *format, va_list args) {
    char *buffer = str;
    int count = 0;
    
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd':
                case 'i': {
                    int num = va_arg(args, int);
                    if (num < 0) {
                        *buffer++ = '-';
                        num = -num;
                        count++;
                    }
                    print_number_to_buffer(&buffer, num, 10);
                    count++;
                    break;
                }
                case 'u':
                    print_number_to_buffer(&buffer, va_arg(args, unsigned int), 10);
                    count++;
                    break;
                case 'x':
                    print_number_to_buffer(&buffer, va_arg(args, unsigned int), 16);
                    count++;
                    break;
                case 'X':
                    print_number_to_buffer(&buffer, va_arg(args, unsigned int), 16);
                    count++;
                    break;
                case 'c':
                    *buffer++ = va_arg(args, int);
                    count++;
                    break;
                case 's': {
                    char *s = va_arg(args, char*);
                    if (s) {
                        while (*s) {
                            *buffer++ = *s++;
                        }
                        count++;
                    }
                    break;
                }
                case '%':
                    *buffer++ = '%';
                    count++;
                    break;
                default:
                    *buffer++ = '%';
                    *buffer++ = *format;
                    count += 2;
                    break;
            }
        } else {
            *buffer++ = *format;
            count++;
        }
        format++;
    }
    
    *buffer = '\0';
    return count;
}

int sprintf(char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsprintf(str, format, args);
    va_end(args);
    return result;
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

int vsscanf(const char *str, const char *format, va_list args) {
    // Basic stub implementation
    (void)str;
    (void)format; 
    (void)args;
    return 0;
}

int sscanf(const char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsscanf(str, format, args);
    va_end(args);
    return result;
}

int scanf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    static char input_buffer[256];
    int matched = 0;
    
    // Simple scanf implementation - only handles %s for now
    if (strcmp(format, "%s") == 0) {
        char *str_ptr = va_arg(args, char*);
        gets_simple(input_buffer, sizeof(input_buffer));
        strcpy(str_ptr, input_buffer);
        matched = 1;
    }
    
    va_end(args);
    return matched;
}