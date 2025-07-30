#ifndef MEMORY_PAGING_H
#define MEMORY_PAGING_H

#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 4096
#define PAGE_ENTRIES 1024

// Page directory/table entry flags
#define PAGE_PRESENT    0x01
#define PAGE_WRITABLE   0x02
#define PAGE_USER       0x04
#define PAGE_ACCESSED   0x20
#define PAGE_DIRTY      0x40

typedef struct {
    uint32_t pages[PAGE_ENTRIES];
} page_table_t;

typedef struct {
    page_table_t *tables[PAGE_ENTRIES];
    uint32_t tables_physical[PAGE_ENTRIES];
    uint32_t physical_addr;
} page_directory_t;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;

// Function prototypes
void paging_init(void);
void switch_page_directory(page_directory_t *dir);
page_directory_t *create_page_directory(void);
void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags);
void unmap_page(uint32_t virtual_addr);
uint32_t get_physical_address(uint32_t virtual_addr);

// Frame management functions
void set_frame(uint32_t frame_addr);
void clear_frame(uint32_t frame_addr);
uint32_t test_frame(uint32_t frame_addr);
uint32_t first_free_frame(void);
void alloc_frame(uint32_t virtual_addr, bool is_kernel, bool is_writable);

#endif