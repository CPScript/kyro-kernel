#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include "paging.h"

#define MAX_PROCESSES 256
#define PROCESS_NAME_MAX 32
#define KERNEL_STACK_SIZE 8192

typedef enum {
    PROCESS_RUNNING,
    PROCESS_READY,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

typedef struct {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp, esp;
    uint32_t eip, cs, ss, eflags;
    uint32_t cr3; // Page directory
} cpu_state_t;

typedef struct process {
    uint32_t pid;
    uint32_t ppid; // Parent process ID
    char name[PROCESS_NAME_MAX];
    process_state_t state;
    
    cpu_state_t cpu_state;
    page_directory_t *page_directory;
    
    uint32_t kernel_stack;
    uint32_t user_stack;
    
    uint32_t priority;
    uint32_t time_slice;
    uint32_t time_used;
    
    struct process *next;
    struct process *parent;
    struct process **children;
    uint32_t child_count;
} process_t;

extern process_t *current_process;
extern process_t *process_list;

void process_init(void);
process_t *create_process(const char *name, void (*entry_point)(void), bool kernel_mode);
void destroy_process(process_t *proc);
void schedule(void);
void switch_task(process_t *next);
uint32_t fork(void);
void exit(int status);
int wait(int *status);

#endif
