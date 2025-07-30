#include <stdarg.h>
#include "kernel.h"

int vsprintf(char *str, const char *format, __builtin_va_list ap) {
    strcpy(str, format);
    return strlen(format);
}

int vsscanf(const char *str, const char *format, __builtin_va_list ap) {
    (void)str;
    (void)format;
    (void)ap;
    return 0;
}

int sprintf(char *str, const char *format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);
    int result = vsprintf(str, format, args);
    __builtin_va_end(args);
    return result;
}

int sscanf(const char *str, const char *format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);
    int result = vsscanf(str, format, args);
    __builtin_va_end(args);
    return result;
}
