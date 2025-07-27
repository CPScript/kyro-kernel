#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

// Forward declarations for size_t
typedef uint32_t size_t;

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
char *strncpy(char *dest, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);

// Basic I/O
void print_message(const char *message);
void clear_screen(void);
void putchar(char c);

// Standard library functions
int printf(const char *format, ...);
int scanf(const char *format, ...);
int sprintf(char *str, const char *format, ...);
int vsprintf(char *str, const char *format, va_list args);
int sscanf(const char *str, const char *format, ...);
int vsscanf(const char *str, const char *format, va_list args);

// Initialization functions
void init_users(void);
void init_file_system(void);

// Module initialization functions
void fs_init(void);
void terminal_init(void);
void user_init(void);
void shell_init(void);
void package_manager_init(void);
void scripting_init(void);

// Core kernel functions
void kernel_init(void);
void kernel_main(void);

// Process management (forward declarations)
struct process;
typedef struct process process_t;
void schedule(void);
process_t *create_process(const char *name, void (*entry_point)(void), bool kernel_mode);
void alloc_frame(uint32_t addr, bool is_kernel, bool is_writable);

// Page directory forward declaration
struct page_directory;
typedef struct page_directory page_directory_t;
extern page_directory_t *kernel_directory;
page_directory_t *create_page_directory(void);

#endif
