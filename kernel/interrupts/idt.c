#include "interrupts/idt.h"
#include "kernel.h"
#include <string.h>

idt_entry_t idt[IDT_ENTRIES];
idt_ptr_t idt_ptr;

// Forward declarations for print_message
extern void print_message(const char *message);

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
    extern void divide_error_handler_asm(void);
    extern void general_protection_fault_handler_asm(void);
    extern void page_fault_handler_asm(void);
    
    idt_set_gate(0, (uint32_t)divide_error_handler_asm, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    idt_set_gate(13, (uint32_t)general_protection_fault_handler_asm, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    idt_set_gate(14, (uint32_t)page_fault_handler_asm, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    
    // Hardware interrupts (32-47)
    extern void timer_handler_asm(void);
    extern void keyboard_handler_asm(void);
    
    idt_set_gate(32, (uint32_t)timer_handler_asm, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    idt_set_gate(33, (uint32_t)keyboard_handler_asm, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32);
    
    load_idt();
}

// Assembly stub for loading IDT
void load_idt(void) {
    asm volatile("lidt %0" : : "m"(idt_ptr));
}

// Exception handlers - properly implemented
void divide_error_handler(void) {
    print_message("EXCEPTION: Divide by zero error\n");
    print_message("System halted.\n");
    asm volatile("cli; hlt");
}

void debug_handler(void) {
    print_message("EXCEPTION: Debug exception\n");
    print_message("System halted.\n");
    asm volatile("cli; hlt");
}

void nmi_handler(void) {
    print_message("EXCEPTION: Non-maskable interrupt\n");
    print_message("System halted.\n");
    asm volatile("cli; hlt");
}

void breakpoint_handler(void) {
    print_message("EXCEPTION: Breakpoint exception\n");
    print_message("System halted.\n");
    asm volatile("cli; hlt");
}

void general_protection_fault_handler(void) {
    print_message("EXCEPTION: General Protection Fault\n");
    print_message("This usually indicates a segmentation violation.\n");
    print_message("System halted.\n");
    asm volatile("cli; hlt");
}

void page_fault_handler(void) {
    uint32_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
    
    print_message("EXCEPTION: Page Fault\n");
    print_message("Faulting address: 0x");
    
    // Simple hex print
    char hex[] = "0123456789ABCDEF";
    char addr_str[9];
    for (int i = 7; i >= 0; i--) {
        addr_str[7-i] = hex[(faulting_address >> (i*4)) & 0xF];
    }
    addr_str[8] = '\0';
    print_message(addr_str);
    print_message("\n");
    
    print_message("System halted.\n");
    asm volatile("cli; hlt");
}

// General fault handler
void fault_handler(void) {
    print_message("FAULT: Unhandled system exception occurred\n");
    print_message("System halted.\n");
    asm volatile("cli; hlt");
}