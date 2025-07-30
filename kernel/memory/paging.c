#include "memory/paging.h"
#include "kernel.h"
#include <string.h>
#include <stddef.h>

page_directory_t *kernel_directory = NULL;
page_directory_t *current_directory = NULL;

uint32_t placement_address = 0x100000; 

// Physical memory allocator
static uint32_t *frames = NULL;
static uint32_t nframes = 0;

// Forward declarations for missing functions
void set_frame(uint32_t frame_addr);
void clear_frame(uint32_t frame_addr);
uint32_t test_frame(uint32_t frame_addr);
uint32_t first_free_frame(void);

page_directory_t *create_page_directory(void) {
    uint32_t phys_addr;
    page_directory_t *dir = (page_directory_t*)kmalloc_ap(sizeof(page_directory_t), &phys_addr);
    memset(dir, 0, sizeof(page_directory_t));
    dir->physical_addr = phys_addr;
    return dir;
}

void paging_init(void) {
    printf("Initializing paging...\n");
    
    // let's use a very simple identity mapping
    // This avoids complex page fault handling during boot
    
    // Initialize frame bitmap for 16MB of RAM
    nframes = 0x1000000 / PAGE_SIZE; // 16MB / 4KB
    frames = (uint32_t*)kmalloc(nframes / 8); // 1 bit per frame
    memset(frames, 0, nframes / 8);
    
    // Create kernel page directory
    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;
    
    // Identity map first 8MB
    printf("Identity mapping first 8MB...\n");
    for (uint32_t i = 0; i < 0x800000; i += PAGE_SIZE) {
        map_page(i, i, PAGE_PRESENT | PAGE_WRITABLE);
    }
    
    printf("Switching to kernel page directory...\n");
    switch_page_directory(kernel_directory);
    
    printf("Enabling paging...\n");
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set PG bit
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
    
    printf("Paging enabled successfully\n");
}

void switch_page_directory(page_directory_t *dir) {
    current_directory = dir;
    asm volatile("mov %0, %%cr3" : : "r"(dir->physical_addr));
}

void map_page(uint32_t virtual_addr, uint32_t physical_addr, uint32_t flags) {
    uint32_t page_dir_index = virtual_addr >> 22;
    uint32_t page_table_index = (virtual_addr >> 12) & 0x3FF;
    
    if (!current_directory->tables[page_dir_index]) {
        // Create new page table
        uint32_t tmp;
        current_directory->tables[page_dir_index] = 
            (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(current_directory->tables[page_dir_index], 0, sizeof(page_table_t));
        current_directory->tables_physical[page_dir_index] = tmp | PAGE_PRESENT | PAGE_WRITABLE;
        if (!(flags & PAGE_USER)) {
            current_directory->tables_physical[page_dir_index] |= PAGE_USER;
        }
    }
    
    current_directory->tables[page_dir_index]->pages[page_table_index] = 
        (physical_addr & 0xFFFFF000) | flags;
}

void unmap_page(uint32_t virtual_addr) {
    uint32_t page_dir_index = virtual_addr >> 22;
    uint32_t page_table_index = (virtual_addr >> 12) & 0x3FF;
    
    if (current_directory->tables[page_dir_index]) {
        current_directory->tables[page_dir_index]->pages[page_table_index] = 0;
    }
}

uint32_t get_physical_address(uint32_t virtual_addr) {
    uint32_t page_dir_index = virtual_addr >> 22;
    uint32_t page_table_index = (virtual_addr >> 12) & 0x3FF;
    uint32_t page_offset = virtual_addr & 0xFFF;
    
    if (!current_directory->tables[page_dir_index]) {
        return 0; // Page table doesn't exist
    }
    
    uint32_t page_entry = current_directory->tables[page_dir_index]->pages[page_table_index];
    if (!(page_entry & PAGE_PRESENT)) {
        return 0; // Page not present
    }
    
    return (page_entry & 0xFFFFF000) | page_offset;
}

void set_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / PAGE_SIZE;
    if (frame >= nframes) return;
    uint32_t idx = frame / 32;
    uint32_t off = frame % 32;
    frames[idx] |= (0x1 << off);
}

void clear_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / PAGE_SIZE;
    if (frame >= nframes) return;
    uint32_t idx = frame / 32;
    uint32_t off = frame % 32;
    frames[idx] &= ~(0x1 << off);
}

uint32_t test_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / PAGE_SIZE;
    if (frame >= nframes) return 1; // Assume allocated if out of range
    uint32_t idx = frame / 32;
    uint32_t off = frame % 32;
    return (frames[idx] & (0x1 << off));
}

uint32_t first_free_frame(void) {
    for (uint32_t i = 0; i < nframes / 32; i++) {
        if (frames[i] != 0xFFFFFFFF) {
            for (uint32_t j = 0; j < 32; j++) {
                if (!(frames[i] & (0x1 << j))) {
                    return i * 32 + j;
                }
            }
        }
    }
    return (uint32_t)-1; // No free frames
}

void alloc_frame(uint32_t virtual_addr, bool is_kernel, bool is_writable) {
    uint32_t physical_addr = first_free_frame() * PAGE_SIZE;
    if (physical_addr == (uint32_t)-1) {
        printf("ERROR: Out of memory in alloc_frame\n");
        return;
    }
    
    set_frame(physical_addr);
    
    uint32_t flags = PAGE_PRESENT;
    if (is_writable) flags |= PAGE_WRITABLE;
    if (!is_kernel) flags |= PAGE_USER;
    
    map_page(virtual_addr, physical_addr, flags);
}