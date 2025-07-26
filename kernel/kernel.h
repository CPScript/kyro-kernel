#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stdbool.h>

// Forward declarations for size_t
typedef unsigned int size_t;

// Memory management functions
uint32_t kmalloc(uint32_t size);
uint32_t kmalloc_a(uint32_t size);
uint32_t kmalloc_p(uint32_t size, uint32_t *phys_addr);
uint32_t kmalloc_ap(uint32_t size, uint32_t *phys_addr);
void kfree(void *ptr);
void *malloc(size_t size);
void free(void *ptr);
void *memset(void *dest, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);

// String functions
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);

// Basic I/O
void print_message(const char *message);
void clear_screen(void);

// Standard library stubs
int printf(const char *format, ...);
int scanf(const char *format, ...);

// Initialization functions
void init_users(void);
void init_file_system(void);

// Core kernel functions
void kernel_init(void);
void kernel_main(void);

#endif
