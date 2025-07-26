#include <stdint.h>
#include <stdbool.h>

// Forward declarations for size_t
typedef unsigned int size_t;
typedef uintptr_t uintptr_t;

// Memory block structure for simple allocator
typedef struct mem_block {
    size_t size;
    bool is_free;
    struct mem_block *next;
} mem_block_t;

// Simple heap start and current pointer
static uint8_t heap[0x100000]; // 1MB heap
static mem_block_t *heap_start = NULL;
static bool heap_initialized = false;

// Early boot allocator (before paging is enabled)
static uint32_t placement_address = 0x100000; // Start after 1MB

// Early boot allocation functions
uint32_t kmalloc_early(uint32_t size) {
    uint32_t addr = placement_address;
    placement_address += size;
    return addr;
}

// Aligned allocation for page tables
uint32_t kmalloc_a(uint32_t size) {
    if (heap_initialized) {
        // Use regular heap but ensure alignment
        void *ptr = malloc(size + 0x1000); // Extra space for alignment
        uint32_t addr = (uint32_t)ptr;
        if (addr & 0xFFF) {
            addr = (addr & 0xFFFFF000) + 0x1000;
        }
        return addr;
    }
    
    // Align to page boundary (4KB)
    if (placement_address & 0xFFF) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    
    uint32_t addr = placement_address;
    placement_address += size;
    return addr;
}

// Aligned allocation with physical address return
uint32_t kmalloc_ap(uint32_t size, uint32_t *phys_addr) {
    uint32_t addr = kmalloc_a(size);
    if (phys_addr) {
        *phys_addr = addr; // Before paging is enabled, virtual == physical
    }
    return addr;
}

// Regular kernel malloc (integrates with existing heap)
uint32_t kmalloc(uint32_t size) {
    if (heap_initialized) {
        return (uint32_t)malloc(size);
    }
    
    uint32_t addr = placement_address;
    placement_address += size;
    return addr;
}

// Regular kernel malloc with physical address
uint32_t kmalloc_p(uint32_t size, uint32_t *phys_addr) {
    uint32_t addr = kmalloc(size);
    if (phys_addr) {
        *phys_addr = addr; // For now, virtual == physical
    }
    return addr;
}

void kfree(void *ptr) {
    if (heap_initialized && ptr >= (void*)heap && ptr < (void*)(heap + sizeof(heap))) {
        free(ptr);
    }
    // Early allocations can't be freed individually
}

// Initialize the heap
void heap_init() {
    heap_start = (mem_block_t *)heap;
    heap_start->size = sizeof(heap) - sizeof(mem_block_t);
    heap_start->is_free = true;
    heap_start->next = NULL;
    heap_initialized = true;
}

// Find a free block of sufficient size
static mem_block_t *find_free_block(size_t size) {
    mem_block_t *current = heap_start;
    
    while (current) {
        if (current->is_free && current->size >= size) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

// Split a block if it's larger than needed
static void split_block(mem_block_t *block, size_t size) {
    if (block->size > size + sizeof(mem_block_t)) {
        mem_block_t *new_block = (mem_block_t *)((uint8_t *)block + sizeof(mem_block_t) + size);
        new_block->size = block->size - size - sizeof(mem_block_t);
        new_block->is_free = true;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

// Merge adjacent free blocks
static void merge_free_blocks() {
    mem_block_t *current = heap_start;
    
    while (current && current->next) {
        if (current->is_free && current->next->is_free) {
            current->size += current->next->size + sizeof(mem_block_t);
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

// Allocate memory
void *malloc(size_t size) {
    if (!heap_initialized) {
        heap_init();
    }
    
    if (size == 0) {
        return NULL;
    }
    
    // Align size to 8-byte boundary
    size = (size + 7) & ~7;
    
    mem_block_t *block = find_free_block(size);
    if (!block) {
        return NULL; // Out of memory
    }
    
    block->is_free = false;
    split_block(block, size);
    
    return (void *)((uint8_t *)block + sizeof(mem_block_t));
}

// Free memory
void free(void *ptr) {
    if (!ptr) {
        return;
    }
    
    // Check if pointer is within our heap
    if (ptr < (void*)heap || ptr >= (void*)(heap + sizeof(heap))) {
        return; // Not our memory
    }
    
    mem_block_t *block = (mem_block_t *)((uint8_t *)ptr - sizeof(mem_block_t));
    block->is_free = true;
    
    merge_free_blocks();
}

// Reallocate memory
void *realloc(void *ptr, size_t size) {
    if (!ptr) {
        return malloc(size);
    }
    
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    mem_block_t *block = (mem_block_t *)((uint8_t *)ptr - sizeof(mem_block_t));
    
    if (block->size >= size) {
        return ptr; // Current block is sufficient
    }
    
    void *new_ptr = malloc(size);
    if (!new_ptr) {
        return NULL;
    }
    
    memcpy(new_ptr, ptr, block->size);
    free(ptr);
    
    return new_ptr;
}

// Allocate zeroed memory
void *calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void *ptr = malloc(total_size);
    
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    
    return ptr;
}

// Memory manipulation functions
void *memset(void *dest, int c, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    uint8_t value = (uint8_t)c;
    
    // Optimized for 8-byte alignment
    while (n >= 8 && ((uintptr_t)d & 7) == 0) {
        uint64_t pattern = ((uint64_t)value << 56) | ((uint64_t)value << 48) |
                          ((uint64_t)value << 40) | ((uint64_t)value << 32) |
                          ((uint64_t)value << 24) | ((uint64_t)value << 16) |
                          ((uint64_t)value << 8) | value;
        *(uint64_t *)d = pattern;
        d += 8;
        n -= 8;
    }
    
    // Handle remaining bytes
    while (n--) {
        *d++ = value;
    }
    
    return dest;
}

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    
    // Optimized for 8-byte alignment
    while (n >= 8 && ((uintptr_t)d & 7) == 0 && ((uintptr_t)s & 7) == 0) {
        *(uint64_t *)d = *(const uint64_t *)s;
        d += 8;
        s += 8;
        n -= 8;
    }
    
    // Handle remaining bytes
    while (n--) {
        *d++ = *s++;
    }
    
    return dest;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    
    if (d < s) {
        // Forward copy
        return memcpy(dest, src, n);
    } else if (d > s) {
        // Backward copy
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;
    
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    
    return 0;
}

void *memchr(const void *s, int c, size_t n) {
    const uint8_t *p = (const uint8_t *)s;
    uint8_t ch = (uint8_t)c;
    
    while (n--) {
        if (*p == ch) {
            return (void *)p;
        }
        p++;
    }
    
    return NULL;
}

// Memory debugging functions
void heap_dump() {
    if (!heap_initialized) {
        return;
    }
    
    mem_block_t *current = heap_start;
    int block_count = 0;
    size_t total_free = 0;
    size_t total_used = 0;
    
    while (current) {
        if (current->is_free) {
            total_free += current->size;
        } else {
            total_used += current->size;
        }
        block_count++;
        current = current->next;
    }
    
    // This would need a working printf implementation (GIGIDY GIGIDY GO!)
    // For now, this is just a placeholder
}

bool is_valid_pointer(void *ptr) {
    return ptr >= (void *)heap && ptr < (void *)(heap + sizeof(heap));
}

// Memory alignment functions
void *aligned_alloc(size_t alignment, size_t size) {
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        return NULL; // Invalid alignment
    }
    
    size_t total_size = size + alignment + sizeof(void *);
    void *raw_ptr = malloc(total_size);
    
    if (!raw_ptr) {
        return NULL;
    }
    
    uintptr_t aligned_addr = ((uintptr_t)raw_ptr + sizeof(void *) + alignment - 1) & ~(alignment - 1);
    void *aligned_ptr = (void *)aligned_addr;
    
    // Store original pointer before aligned pointer
    ((void **)aligned_ptr)[-1] = raw_ptr;
    
    return aligned_ptr;
}

void aligned_free(void *ptr) {
    if (!ptr) {
        return;
    }
    
    void *raw_ptr = ((void **)ptr)[-1];
    free(raw_ptr);
}

// Memory pool allocator for fixed-size objects
typedef struct mem_pool {
    void *memory;
    size_t object_size;
    size_t object_count;
    void *free_list;
} mem_pool_t;

mem_pool_t *pool_create(size_t object_size, size_t object_count) {
    mem_pool_t *pool = malloc(sizeof(mem_pool_t));
    if (!pool) {
        return NULL;
    }
    
    pool->object_size = (object_size + sizeof(void *) - 1) & ~(sizeof(void *) - 1);
    pool->object_count = object_count;
    pool->memory = malloc(pool->object_size * object_count);
    
    if (!pool->memory) {
        free(pool);
        return NULL;
    }
    
    // Initialize free list
    pool->free_list = pool->memory;
    uint8_t *current = (uint8_t *)pool->memory;
    
    for (size_t i = 0; i < object_count - 1; i++) {
        *(void **)current = current + pool->object_size;
        current += pool->object_size;
    }
    
    *(void **)current = NULL; // Last object points to NULL
    
    return pool;
}

void *pool_alloc(mem_pool_t *pool) {
    if (!pool || !pool->free_list) {
        return NULL;
    }
    
    void *obj = pool->free_list;
    pool->free_list = *(void **)obj;
    
    return obj;
}

void pool_free(mem_pool_t *pool, void *obj) {
    if (!pool || !obj) {
        return;
    }
    
    *(void **)obj = pool->free_list;
    pool->free_list = obj;
}

void pool_destroy(mem_pool_t *pool) {
    if (!pool) {
        return;
    }
    
    free(pool->memory);
    free(pool);
}

// Get current placement address (useful for debugging)
uint32_t get_placement_address() {
    return placement_address;
}

// Set placement address (useful for bootloader integration)
void set_placement_address(uint32_t addr) {
    placement_address = addr;
}

// Memory statistics
typedef struct {
    size_t total_heap_size;
    size_t used_heap_size;
    size_t free_heap_size;
    size_t largest_free_block;
    int total_blocks;
    int free_blocks;
    int used_blocks;
} memory_stats_t;

void get_memory_stats(memory_stats_t *stats) {
    if (!stats || !heap_initialized) {
        return;
    }
    
    stats->total_heap_size = sizeof(heap);
    stats->used_heap_size = 0;
    stats->free_heap_size = 0;
    stats->largest_free_block = 0;
    stats->total_blocks = 0;
    stats->free_blocks = 0;
    stats->used_blocks = 0;
    
    mem_block_t *current = heap_start;
    while (current) {
        stats->total_blocks++;
        if (current->is_free) {
            stats->free_blocks++;
            stats->free_heap_size += current->size;
            if (current->size > stats->largest_free_block) {
                stats->largest_free_block = current->size;
            }
        } else {
            stats->used_blocks++;
            stats->used_heap_size += current->size;
        }
        current = current->next;
    }
}

// Check heap integrity
bool check_heap_integrity() {
    if (!heap_initialized) {
        return true; // No heap to check
    }
    
    mem_block_t *current = heap_start;
    uint8_t *heap_end = heap + sizeof(heap);
    
    while (current) {
        // Check if block is within heap bounds
        if ((uint8_t*)current < heap || (uint8_t*)current >= heap_end) {
            return false;
        }
        
        // Check if block size is reasonable
        if (current->size == 0 || current->size > sizeof(heap)) {
            return false;
        }
        
        // Check if next block pointer is valid
        if (current->next && ((uint8_t*)current->next < heap || (uint8_t*)current->next >= heap_end)) {
            return false;
        }
        
        current = current->next;
    }
    
    return true;
}
