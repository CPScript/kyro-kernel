#include "process.h"
#include "kernel.h"
#include <string.h>

process_t *current_process = NULL;
process_t *process_list = NULL;
static uint32_t next_pid = 1;

void process_init(void) {
    // Create initial kernel process
    current_process = create_process("kernel", NULL, true);
    current_process->state = PROCESS_RUNNING;
    process_list = current_process;
}

process_t *create_process(const char *name, void (*entry_point)(void), bool kernel_mode) {
    process_t *proc = kmalloc(sizeof(process_t));
    if (!proc) return NULL;
    
    memset(proc, 0, sizeof(process_t));
    
    proc->pid = next_pid++;
    strncpy(proc->name, name, PROCESS_NAME_MAX - 1);
    proc->state = PROCESS_READY;
    proc->priority = 10;
    proc->time_slice = 10; // 10 timer ticks
    
    // Allocate kernel stack
    proc->kernel_stack = (uint32_t)kmalloc(KERNEL_STACK_SIZE) + KERNEL_STACK_SIZE;
    
    if (!kernel_mode) {
        // Create user address space
        proc->page_directory = create_page_directory();
        
        // Allocate user stack at high memory
        proc->user_stack = 0xC0000000;
        alloc_frame(proc->user_stack - PAGE_SIZE, false, true);
        
        // Set up initial CPU state for user mode
        proc->cpu_state.cs = 0x1B; // User code segment
        proc->cpu_state.ss = 0x23; // User data segment
        proc->cpu_state.eflags = 0x202; // Interrupts enabled
        proc->cpu_state.esp = proc->user_stack;
    } else {
        proc->page_directory = kernel_directory;
        proc->cpu_state.cs = 0x08; // Kernel code segment
        proc->cpu_state.ss = 0x10; // Kernel data segment
        proc->cpu_state.eflags = 0x202;
        proc->cpu_state.esp = proc->kernel_stack;
    }
    
    if (entry_point) {
        proc->cpu_state.eip = (uint32_t)entry_point;
    }
    
    proc->cpu_state.cr3 = proc->page_directory->physical_addr;
    
    // Add to process list
    proc->next = process_list;
    process_list = proc;
    
    return proc;
}

void destroy_process(process_t *proc) {
    if (!proc) return;
    
    // Remove from process list
    if (process_list == proc) {
        process_list = proc->next;
    } else {
        process_t *current = process_list;
        while (current && current->next != proc) {
            current = current->next;
        }
        if (current) {
            current->next = proc->next;
        }
    }
    
    // Free resources
    if (proc->page_directory != kernel_directory) {
        // Free user page directory
        kfree(proc->page_directory);
    }
    
    kfree((void*)(proc->kernel_stack - KERNEL_STACK_SIZE));
    kfree(proc);
}

void schedule(void) {
    if (!current_process) return;
    
    // Simple round-robin scheduler
    process_t *next = current_process->next;
    if (!next) next = process_list;
    
    // Find next ready process
    while (next && next->state != PROCESS_READY && next != current_process) {
        next = next->next;
        if (!next) next = process_list;
    }
    
    if (next && next != current_process && next->state == PROCESS_READY) {
        current_process->state = PROCESS_READY;
        switch_task(next);
    }
}

void switch_task(process_t *next) {
    if (!next || next == current_process) return;
    
    process_t *prev = current_process;
    current_process = next;
    current_process->state = PROCESS_RUNNING;
    
    // Switch page directory
    switch_page_directory(current_process->page_directory);
    
    // Perform context switch (assembly required)
    context_switch(&prev->cpu_state, &current_process->cpu_state);
}

uint32_t fork(void) {
    if (!current_process) return -1;
    
    // Create child process
    process_t *child = create_process(current_process->name, NULL, false);
    if (!child) return -1;
    
    // Copy parent's memory space
    // This requires implementing copy-on-write or full memory copy
    
    // Set up parent-child relationship
    child->ppid = current_process->pid;
    child->parent = current_process;
    
    // Copy CPU state from parent
    memcpy(&child->cpu_state, &current_process->cpu_state, sizeof(cpu_state_t));
    
    // Return 0 to child, child PID to parent
    if (current_process == child) {
        return 0; // Child process
    } else {
        return child->pid; // Parent process
    }
}

void exit(int status) {
    if (!current_process) return;
    
    current_process->state = PROCESS_TERMINATED;
    
    // Wake up parent if waiting
    if (current_process->parent && 
        current_process->parent->state == PROCESS_BLOCKED) {
        current_process->parent->state = PROCESS_READY;
    }
    
    // Schedule next process
    schedule();
}

// Assembly context switch function
asm(
    ".global context_switch\n"
    "context_switch:\n"
    "    push %ebp\n"
    "    mov %esp, %ebp\n"
    "    push %eax\n"
    "    push %ebx\n"
    "    push %ecx\n"
    "    push %edx\n"
    "    push %esi\n"
    "    push %edi\n"
    "    pushf\n"
    "    \n"
    "    mov 8(%ebp), %eax    # prev state\n"
    "    mov %esp, 28(%eax)   # save ESP\n"
    "    \n"
    "    mov 12(%ebp), %eax   # next state\n"
    "    mov 28(%eax), %esp   # restore ESP\n"
    "    mov 32(%eax), %ebx   # restore CR3\n"
    "    mov %ebx, %cr3\n"
    "    \n"
    "    popf\n"
    "    pop %edi\n"
    "    pop %esi\n"
    "    pop %edx\n"
    "    pop %ecx\n"
    "    pop %ebx\n"
    "    pop %eax\n"
    "    pop %ebp\n"
    "    ret\n"
);
