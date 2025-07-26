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
char *strcat(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strncpy(char *dest, const char *src, size_t n);

// Basic I/O
void print_message(const char *message);
void clear_screen(void);

// Standard library functions
int printf(const char *format, ...);
int scanf(const char *format, ...);
int sprintf(char *str, const char *format, ...);
int sscanf(const char *str, const char *format, ...);
int vsprintf(char *str, const char *format, __builtin_va_list ap);
int vsscanf(const char *str, const char *format, __builtin_va_list ap);

// Initialization functions
void init_users(void);
void init_file_system(void);

// System functions
void fs_init(void);
void terminal_init(void);
void user_init(void);
void shell_init(void);
void package_manager_init(void);
void scripting_init(void);

// Process management
void create_process(const char *name, void (*entry_point)(void), bool kernel_mode);
void schedule(void);

// Page directory creation (forward declaration)
struct page_directory;
struct page_directory *create_page_directory(void);
void alloc_frame(uint32_t virtual_addr, bool is_kernel, bool is_writable);

// Core kernel functions
void kernel_init(void);
void kernel_main(void);

// Global variables
extern uint32_t placement_address;

#endif
