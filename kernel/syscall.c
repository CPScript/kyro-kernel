#include <stdint.h>
#include "kernel.h"
#include "interrupts/idt.h"

// System call numbers
#define SYS_EXIT    1
#define SYS_FORK    2
#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE   6
#define SYS_WAITPID 7
#define SYS_CREAT   8
#define SYS_LINK    9
#define SYS_UNLINK  10

// System call handler
void syscall_handler(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    switch (eax) {
        case SYS_EXIT:
            // Handle exit system call
            printf("Process exiting with code: %d\n", ebx);
            break;
            
        case SYS_FORK:
            // Handle fork system call
            printf("Fork system call not implemented\n");
            break;
            
        case SYS_READ:
            // Handle read system call
            printf("Read system call: fd=%d, buf=0x%x, count=%d\n", ebx, ecx, edx);
            break;
            
        case SYS_WRITE:
            // Handle write system call
            if (ebx == 1) { // stdout
                char *str = (char*)ecx;
                for (uint32_t i = 0; i < edx; i++) {
                    if (str[i] == '\0') break;
                    putchar(str[i]);
                }
            }
            break;
            
        default:
            printf("Unknown system call: %d\n", eax);
            break;
    }
}

// Assembly syscall handler wrapper
extern void syscall_handler_asm(void);

void syscall_init(void) {
    // Register system call interrupt (usually INT 0x80)
    idt_set_gate(0x80, (uint32_t)syscall_handler_asm, 0x08, 
                 IDT_FLAG_PRESENT | IDT_FLAG_RING3 | IDT_GATE_INT32);
}

// Assembly wrapper for system call handler
asm(
    ".global syscall_handler_asm\n"
    "syscall_handler_asm:\n"
    "    cli\n"
    "    pusha\n"
    "    push %ds\n"
    "    push %es\n"
    "    push %fs\n"
    "    push %gs\n"
    "    \n"
    "    mov $0x10, %ax\n"  // Load kernel data segment
    "    mov %ax, %ds\n"
    "    mov %ax, %es\n"
    "    mov %ax, %fs\n"
    "    mov %ax, %gs\n"
    "    \n"
    "    push %edx\n"       // 4th parameter
    "    push %ecx\n"       // 3rd parameter
    "    push %ebx\n"       // 2nd parameter
    "    push %eax\n"       // 1st parameter (syscall number)
    "    call syscall_handler\n"
    "    add $16, %esp\n"   // Clean up parameters
    "    \n"
    "    pop %gs\n"
    "    pop %fs\n"
    "    pop %es\n"
    "    pop %ds\n"
    "    popa\n"
    "    sti\n"
    "    iret\n"
);
