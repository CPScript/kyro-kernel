#include "idt.h"
#include <string.h>

idt_entry_t idt[IDT_ENTRIES];
idt_ptr_t idt_ptr;

void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = base & 0xFFFF;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = flags;
}

void idt_init(void) {
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
    idt_ptr.base = (uint32_t)&idt;
    
    // Clear IDT
    memset(&idt, 0, sizeof(idt_entry_t) * IDT_ENTRIES);
    
    // Set up exception handlers (0-31)
    idt_set_gate(0, (uint32_t)divide_error_handler, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    idt_set_gate(1, (uint32_t)debug_handler, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    idt_set_gate(2, (uint32_t)nmi_handler, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    idt_set_gate(3, (uint32_t)breakpoint_handler, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    idt_set_gate(13, (uint32_t)general_protection_fault_handler, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    idt_set_gate(14, (uint32_t)page_fault_handler, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    
    // Hardware interrupts (32-47)
    idt_set_gate(32, (uint32_t)timer_handler, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    idt_set_gate(33, (uint32_t)keyboard_handler, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    
    load_idt();
}

// Assembly stub for loading IDT
void load_idt(void) {
    asm volatile("lidt %0" : : "m"(idt_ptr));
}

// Exception handlers (basic implementations)
void divide_error_handler(void) {
    // Handle divide by zero
    asm volatile("cli; hlt");
}

void general_protection_fault_handler(void) {
    // Handle GPF
    asm volatile("cli; hlt");
}

void page_fault_handler(void) {
    // Handle page fault
    asm volatile("cli; hlt");
}
