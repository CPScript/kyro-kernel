#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include "kernel.h"
#include "drivers/keyboard.h"

// External putchar from kernel.c
extern void putchar(char c);

// Forward declaration
void puts(const char *str);

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

static void print_number_to_buffer(char **buffer, unsigned long num, int base) {
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

static void print_number(unsigned long num, int base) {
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
                    long num = va_arg(args, long);
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
                    print_number_to_buffer(&buffer, va_arg(args, unsigned long), 10);
                    count++;
                    break;
                case 'x':
                    print_number_to_buffer(&buffer, va_arg(args, unsigned long), 16);
                    count++;
                    break;
                case 'X':
                    print_number_to_buffer(&buffer, va_arg(args, unsigned long), 16);
                    count++;
                    break;
                case 'p': {
                    *buffer++ = '0';
                    *buffer++ = 'x';
                    print_number_to_buffer(&buffer, (unsigned long)va_arg(args, void*), 16);
                    count++;
                    break;
                }
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
                    } else {
                        char *null_str = "(null)";
                        while (*null_str) {
                            *buffer++ = *null_str++;
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
                    long num = va_arg(args, long);
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
                    print_number(va_arg(args, unsigned long), 10);
                    count++;
                    break;
                case 'x':
                    print_number(va_arg(args, unsigned long), 16);
                    count++;
                    break;
                case 'X':
                    print_number(va_arg(args, unsigned long), 16);
                    count++;
                    break;
                case 'p':
                    putchar('0');
                    putchar('x');
                    print_number((unsigned long)va_arg(args, void*), 16);
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
                    } else {
                        puts("(null)");
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
    // Basic stub implementation - you can expand this
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