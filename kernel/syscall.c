#include "syscall.h"
#include "interrupts/idt.h"
#include "kernel.h"

static syscall_handler_t syscall_table[256];

void syscall_init(void) {
    // Set up system call interrupt (interrupt 0x80)
    idt_set_gate(0x80, (uint32_t)syscall_handler_asm, 0x08, IDT_FLAG_PRESENT | IDT_GATE_INT32 | IDT_FLAG_RING3);
    
    // Initialize system call table
    syscall_table[SYS_EXIT] = (syscall_handler_t)sys_exit;
    syscall_table[SYS_FORK] = (syscall_handler_t)sys_fork;
    syscall_table[SYS_READ] = (syscall_handler_t)sys_read;
    syscall_table[SYS_WRITE] = (syscall_handler_t)sys_write;
    syscall_table[SYS_OPEN] = (syscall_handler_t)sys_open;
    syscall_table[SYS_CLOSE] = (syscall_handler_t)sys_close;
    syscall_table[SYS_GETPID] = (syscall_handler_t)sys_getpid;
    syscall_table[SYS_BRK] = (syscall_handler_t)sys_brk;
}

uint32_t syscall_dispatch(uint32_t call_num, uint32_t arg1, uint32_t arg2, uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    if (call_num >= 256 || !syscall_table[call_num]) {
        return -1; // Invalid system call
    }
    
    return syscall_table[call_num](arg1, arg2, arg3, arg4, arg5);
}

// System call implementations
uint32_t sys_exit(uint32_t status) {
    // Exit current process
    print_message("Process exiting\n");
    return 0;
}

uint32_t sys_fork(void) {
    // Fork current process
    print_message("Fork not implemented\n");
    return -1;
}

uint32_t sys_read(uint32_t fd, uint32_t buffer, uint32_t count) {
    // Read from file descriptor
    (void)fd;
    (void)buffer;
    (void)count;
    return 0;
}

uint32_t sys_write(uint32_t fd, uint32_t buffer, uint32_t count) {
    // Write to file descriptor
    if (fd == 1 || fd == 2) { // stdout or stderr
        char *str = (char*)buffer;
        for (uint32_t i = 0; i < count; i++) {
            if (str[i] == 0) break;
            putchar(str[i]);
        }
        return count;
    }
    return -1;
}

uint32_t sys_open(uint32_t pathname, uint32_t flags, uint32_t mode) {
    // Open file
    (void)pathname;
    (void)flags;
    (void)mode;
    return -1;
}

uint32_t sys_close(uint32_t fd) {
    // Close file descriptor
    (void)fd;
    return 0;
}

uint32_t sys_getpid(void) {
    // Get process ID
    return 1; // Stub
}

uint32_t sys_brk(uint32_t addr) {
    // Set program break
    (void)addr;
    return 0;
}

// Assembly wrapper for system call handler
asm(
    ".global syscall_handler_asm\n"
    "syscall_handler_asm:\n"
    "    cli\n"
    "    push %eax\n"      // Save registers
    "    push %ecx\n"
    "    push %edx\n"
    "    push %ebx\n"
    "    push %esp\n"
    "    push %ebp\n"
    "    push %esi\n"
    "    push %edi\n"
    "    \n"
    "    push %edi\n"      // arg5
    "    push %esi\n"      // arg4  
    "    push %edx\n"      // arg3
    "    push %ecx\n"      // arg2
    "    push %ebx\n"      // arg1
    "    push %eax\n"      // syscall number
    "    call syscall_dispatch\n"
    "    add $24, %esp\n"  // Clean up arguments
    "    \n"
    "    pop %edi\n"       // Restore registers
    "    pop %esi\n"
    "    pop %ebp\n"
    "    add $4, %esp\n"   // Skip ESP
    "    pop %ebx\n"
    "    pop %edx\n"
    "    pop %ecx\n"
    "    add $4, %esp\n"   // Skip EAX (return value)
    "    sti\n"
    "    iret\n"
);
