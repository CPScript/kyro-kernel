#include "idt.h"
#include "kernel.h"
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

void divide_error_handler(void) {
    // Handle divide by zero exception
    print_message("EXCEPTION: Divide by zero\n");
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

void debug_handler(void) {
    print_message("EXCEPTION: Debug\n");
    asm volatile("cli; hlt");
}

void nmi_handler(void) {
    print_message("EXCEPTION: NMI\n");
    asm volatile("cli; hlt");
}

void breakpoint_handler(void) {
    print_message("EXCEPTION: Breakpoint\n");
    asm volatile("cli; hlt");
}

void general_protection_fault_handler(void) {
    print_message("EXCEPTION: General Protection Fault\n");
    asm volatile("cli; hlt");
}

void page_fault_handler(void) {
    print_message("EXCEPTION: Page Fault\n");
    asm volatile("cli; hlt");
}

// Hardware interrupt handlers
void timer_handler_asm(void);
void keyboard_handler_asm(void);

// Assembly wrappers for interrupt handlers
asm(
    ".global divide_error_handler_asm\n"
    "divide_error_handler_asm:\n"
    "    cli\n"
    "    push $0\n"    // Error code (none for divide error)
    "    push $0\n"    // Interrupt number
    "    jmp isr_common_stub\n"
    
    ".global general_protection_fault_handler_asm\n" 
    "general_protection_fault_handler_asm:\n"
    "    cli\n"
    "    push $13\n"   // Interrupt number
    "    jmp isr_common_stub\n"
    
    ".global page_fault_handler_asm\n"
    "page_fault_handler_asm:\n"
    "    cli\n"
    "    push $14\n"   // Interrupt number  
    "    jmp isr_common_stub\n"
    
    ".global timer_handler_asm\n"
    "timer_handler_asm:\n"
    "    cli\n"
    "    pusha\n"
    "    call timer_handler\n"
    "    popa\n"
    "    sti\n"
    "    iret\n"
    
    ".global keyboard_handler_asm\n"
    "keyboard_handler_asm:\n"
    "    cli\n"
    "    pusha\n" 
    "    call keyboard_interrupt_handler\n"
    "    popa\n"
    "    sti\n"
    "    iret\n"
    
    "isr_common_stub:\n"
    "    pusha\n"
    "    push %ds\n"
    "    push %es\n"
    "    push %fs\n"
    "    push %gs\n"
    "    mov $0x10, %ax\n"  // Load kernel data segment
    "    mov %ax, %ds\n"
    "    mov %ax, %es\n"
    "    mov %ax, %fs\n"
    "    mov %ax, %gs\n"
    "    call fault_handler\n"  // Call C handler
    "    pop %gs\n"
    "    pop %fs\n"
    "    pop %es\n"
    "    pop %ds\n"
    "    popa\n"
    "    add $8, %esp\n"     // Clean up error code and int number
    "    iret\n"
);

// C fault handler
void fault_handler(void) {
    print_message("FAULT: System exception occurred\n");
    asm volatile("cli; hlt");  // Halt system
}
