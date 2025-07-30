#include <stdint.h>
#include <string.h>
#include <stddef.h>

// Simple heap allocator
static uint32_t heap_start = 0x100000; // 1MB
static uint32_t heap_current = 0x100000;
static uint32_t heap_end = 0x200000; // 2MB

uint32_t kmalloc(uint32_t size) {
    if (heap_current + size >= heap_end) {
        return 0; // Out of memory
    }
    uint32_t addr = heap_current;
    heap_current += size;
    return addr;
}

uint32_t kmalloc_a(uint32_t size) {
    // Align to page boundary (4KB)
    heap_current = (heap_current + 0xFFF) & ~0xFFF;
    return kmalloc(size);
}

uint32_t kmalloc_p(uint32_t size, uint32_t *phys_addr) {
    uint32_t addr = kmalloc(size);
    if (phys_addr) {
        *phys_addr = addr; // In this simple implementation, virtual = physical
    }
    return addr;
}

uint32_t kmalloc_ap(uint32_t size, uint32_t *phys_addr) {
    uint32_t addr = kmalloc_a(size);
    if (phys_addr) {
        *phys_addr = addr;
    }
    return addr;
}

void kfree(void *ptr) {
    // Simple implementation - no actual freeing
    // I need to implement a proper free list but rn we arnt even able to compile this OS
}

void *malloc(size_t size) {
    return (void*)kmalloc(size);
}

void free(void *ptr) {
    kfree(ptr);
}

void *memset(void *dest, int c, size_t n) {
    uint8_t *d = (uint8_t*)dest;
    for (size_t i = 0; i < n; i++) {
        d[i] = (uint8_t)c;
    }
    return dest;
}

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
    
    if (d < s) {
        for (size_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else {
        for (size_t i = n; i > 0; i--) {
            d[i-1] = s[i-1];
        }
    }
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t*)s1;
    const uint8_t *p2 = (const uint8_t*)s2;
    
    for (size_t i = 0; i < n; i++) {
        if (p1[i] < p2[i]) return -1;
        if (p1[i] > p2[i]) return 1;
    }
    return 0;
}

void *memchr(const void *s, int c, size_t n) {
    const uint8_t *p = (const uint8_t*)s;
    for (size_t i = 0; i < n; i++) {
        if (p[i] == (uint8_t)c) {
            return (void*)(p + i);
        }
    }
    return NULL;
}
