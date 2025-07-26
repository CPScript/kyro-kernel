#ifndef IDT_H
#define IDT_H

#include <stdint.h>

#define IDT_ENTRIES 256

// IDT gate types
#define IDT_GATE_TASK     0x5
#define IDT_GATE_INT16    0x6
#define IDT_GATE_TRAP16   0x7
#define IDT_GATE_INT32    0xE
#define IDT_GATE_TRAP32   0xF

// IDT flags
#define IDT_FLAG_PRESENT  0x80
#define IDT_FLAG_RING0    0x00
#define IDT_FLAG_RING3    0x60

typedef struct {
    uint16_t offset_low;    // Lower 16 bits of handler address
    uint16_t selector;      // Code segment selector
    uint8_t  zero;          // Reserved, must be zero
    uint8_t  type_attr;     // Type and attributes
    uint16_t offset_high;   // Upper 16 bits of handler address
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

extern idt_entry_t idt[IDT_ENTRIES];
extern idt_ptr_t idt_ptr;

void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);
void load_idt(void);

#endif
