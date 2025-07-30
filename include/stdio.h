#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>
#include <stdbool.h>

typedef struct {
    int fd; // File descriptor
    char *buffer; // Buffer for reading/writing
    int buffer_size; // Size of the buffer
    int buffer_index; // Current position in the buffer
} FILE;

FILE *fopen(const char *filename, const char *mode);
int fclose(FILE *fp);
char *fgets(char *str, int size, FILE *fp);
int fprintf(FILE *fp, const char *format, ...);
int fscanf(FILE *fp, const char *format, ...);
int printf(const char *format, ...);
int scanf(const char *format, ...);
int sprintf(char *str, const char *format, ...);
int sscanf(const char *str, const char *format, ...);

#endif
