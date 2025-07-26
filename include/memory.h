#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// Memory constants
#define PAGE_SIZE 4096
#define PAGE_ALIGN_DOWN(addr) ((addr) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(addr) (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))
#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIRECTORY 1024
#define PAGE_DIRECTORY_INDEX(addr) (((addr) >> 22) & 0x3FF)
#define PAGE_TABLE_INDEX(addr) (((addr) >> 12) & 0x3FF)
#define PAGE_GET_PHYSICAL_ADDRESS(page) ((page) & ~0xFFF)

// Memory layout constants
#define KERNEL_START 0x100000          // 1MB - where kernel starts
#define KERNEL_HEAP_START 0x400000     // 4MB - kernel heap start
#define KERNEL_HEAP_SIZE 0x1000000     // 16MB - kernel heap size
#define USER_SPACE_START 0x40000000    // 1GB - user space start
#define PHYSICAL_MEMORY_OFFSET 0xC0000000  // 3GB - physical memory mapping

// Page flags
#define PAGE_PRESENT    0x001
#define PAGE_WRITABLE   0x002
#define PAGE_USER       0x004
#define PAGE_WRITE_THROUGH 0x008
#define PAGE_CACHE_DISABLED 0x010
#define PAGE_ACCESSED   0x020
#define PAGE_DIRTY      0x040
#define PAGE_SIZE_4MB   0x080
#define PAGE_GLOBAL     0x100
#define PAGE_AVAILABLE1 0x200
#define PAGE_AVAILABLE2 0x400
#define PAGE_AVAILABLE3 0x800

// Memory regions
typedef enum {
    MEMORY_AVAILABLE = 1,
    MEMORY_RESERVED = 2,
    MEMORY_ACPI_RECLAIMABLE = 3,
    MEMORY_ACPI_NVS = 4,
    MEMORY_BAD = 5
} memory_region_type_t;

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t extended_attributes;
} __attribute__((packed)) memory_map_entry_t;

// Physical memory manager structures
typedef struct {
    uint32_t *bitmap;
    uint32_t total_frames;
    uint32_t used_frames;
    uint32_t free_frames;
    uint32_t bitmap_size;
} physical_memory_manager_t;

// Page directory and table entries
typedef uint32_t page_directory_entry_t;
typedef uint32_t page_table_entry_t;

typedef struct {
    page_table_entry_t pages[PAGES_PER_TABLE];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

typedef struct {
    page_directory_entry_t tables[PAGES_PER_DIRECTORY];
} __attribute__((aligned(PAGE_SIZE))) page_directory_t;

// Virtual memory manager
typedef struct {
    page_directory_t *page_directory;
    page_table_t *page_tables[PAGES_PER_DIRECTORY];
    uint32_t physical_address;
} virtual_address_space_t;

// Heap block structure for memory allocation
typedef struct heap_block {
    size_t size;
    bool free;
    struct heap_block *next;
    struct heap_block *prev;
    uint32_t magic;  // For corruption detection
} heap_block_t;

#define HEAP_MAGIC 0xDEADBEEF
#define MIN_BLOCK_SIZE sizeof(heap_block_t)

// Heap manager
typedef struct {
    void *heap_start;
    void *heap_end;
    size_t heap_size;
    heap_block_t *free_list;
    uint32_t total_allocated;
    uint32_t total_blocks;
} heap_manager_t;

// Function prototypes

// Physical memory management
void pmm_init(memory_map_entry_t *memory_map, uint32_t memory_map_length);
uint32_t pmm_alloc_frame(void);
void pmm_free_frame(uint32_t frame_addr);
uint32_t pmm_get_total_memory(void);
uint32_t pmm_get_used_memory(void);
uint32_t pmm_get_free_memory(void);
bool pmm_is_frame_allocated(uint32_t frame_addr);

// Virtual memory management
void vmm_init(void);
virtual_address_space_t *vmm_create_address_space(void);
void vmm_destroy_address_space(virtual_address_space_t *space);
bool vmm_map_page(virtual_address_space_t *space, uint32_t virtual_addr, 
                  uint32_t physical_addr, uint32_t flags);
void vmm_unmap_page(virtual_address_space_t *space, uint32_t virtual_addr);
uint32_t vmm_get_physical_address(virtual_address_space_t *space, uint32_t virtual_addr);
void vmm_switch_address_space(virtual_address_space_t *space);
page_table_t *vmm_get_page_table(virtual_address_space_t *space, uint32_t virtual_addr, bool create);

// Kernel heap management
void heap_init(void);
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t size);
void heap_dump(void);
size_t heap_get_allocated_size(void *ptr);

// Memory utility functions
void *memset(void *dest, int value, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
int memcmp(const void *ptr1, const void *ptr2, size_t count);
void *memmove(void *dest, const void *src, size_t count);

// Page fault handler
void page_fault_handler(uint32_t error_code, uint32_t virtual_address);

// Memory debugging
void memory_dump_stats(void);
void memory_check_corruption(void);

// Global variables
extern physical_memory_manager_t pmm;
extern virtual_address_space_t *kernel_address_space;
extern heap_manager_t kernel_heap;

// Assembly functions (implemented in memory_asm.s)
extern void enable_paging(uint32_t page_directory_physical);
extern void disable_paging(void);
extern uint32_t get_page_directory(void);
extern void invalidate_page(uint32_t virtual_address);
extern void flush_tlb(void);

#endif // MEMORY_H
