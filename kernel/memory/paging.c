#include "paging.h"
#include "kernel.h"
#include <string.h>

page_directory_t *kernel_directory = NULL;
page_directory_t *current_directory = NULL;

uint32_t placement_address = 0x100000; // 1MB mark

page_directory_t *create_page_directory(void) {
    page_directory_t *dir = (page_directory_t*)kmalloc_ap(sizeof(page_directory_t), &dir->physical_addr);
    memset(dir, 0, sizeof(page_directory_t));
    return dir;
}

void paging_init(void) {
    // Create kernel page directory
    kernel_directory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    
    // Identity map first 4MB (kernel space)
    for (uint32_t i = 0; i < 0x400000; i += PAGE_SIZE) {
        map_page(i, i, PAGE_PRESENT | PAGE_WRITABLE);
    }
    
    // Enable paging
    switch_page_directory(kernel_directory);
    
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set PG bit
    asm volatile("mov %0, %%cr0" : : "r"(cr0));
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
        current_directory->tables[page_dir_index] = 
            (page_table_t *)kmalloc_ap(sizeof(page_table_t), 
                                      &current_directory->tables_physical[page_dir_index]);
        memset(current_directory->tables[page_dir_index], 0, sizeof(page_table_t));
        current_directory->tables_physical[page_dir_index] |= PAGE_PRESENT | PAGE_WRITABLE | flags;
    }
    
    current_directory->tables[page_dir_index]->pages[page_table_index] = 
        (physical_addr & 0xFFFFF000) | flags;
}

// Physical memory allocator
static uint32_t *frames;
static uint32_t nframes;

void set_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / PAGE_SIZE;
    uint32_t idx = frame / 32;
    uint32_t off = frame % 32;
    frames[idx] |= (0x1 << off);
}

void clear_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / PAGE_SIZE;
    uint32_t idx = frame / 32;
    uint32_t off = frame % 32;
    frames[idx] &= ~(0x1 << off);
}

uint32_t test_frame(uint32_t frame_addr) {
    uint32_t frame = frame_addr / PAGE_SIZE;
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
    return -1; // No free frames
}

void alloc_frame(uint32_t virtual_addr, bool is_kernel, bool is_writable) {
    uint32_t physical_addr = first_free_frame() * PAGE_SIZE;
    if (physical_addr == (uint32_t)-1) {
        // Out of memory
        return;
    }
    
    set_frame(physical_addr);
    
    uint32_t flags = PAGE_PRESENT;
    if (is_writable) flags |= PAGE_WRITABLE;
    if (!is_kernel) flags |= PAGE_USER;
    
    map_page(virtual_addr, physical_addr, flags);
}
