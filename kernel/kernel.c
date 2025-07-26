// Updated to work with changes
#include "kernel.h"
#include "interrupts/idt.h"
#include "memory/paging.h"
#include "timer.h"
#include "syscall.h"
#include "process.h"
#include "drivers/keyboard.h"
#include "drivers/mouse.h"
#include "drivers/network.h"
#include "fs.h"
#include "terminal.h"
#include "user.h"
#include "shell.h"
#include "package_manager.h"
#include "scripting.h"

void clear_screen() {
    char *video_memory = (char *)0xb8000;
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x07;
    }
}

void print_message(const char *message) {
    char *video_memory = (char *)0xb8000;
    static int offset = 0;
    while (*message) {
        video_memory[offset * 2] = *message;
        video_memory[offset * 2 + 1] = 0x07;
        offset++;
        message++;
    }
}

void kernel_init() {
    // Initialize core systems first
    idt_init();
    paging_init();
    timer_init(100); // 100Hz timer
    syscall_init();
    process_init();
    
    // Initialize your existing components
    fs_init();
    terminal_init();
    user_init();
    shell_init();
    package_manager_init();
    scripting_init();
    
    // Initialize drivers
    keyboard_init();
    mouse_init();
    network_init();
    
    // Initialize user system and filesystem
    init_users();
    init_file_system();
    
    print_message("Kyro OS - All systems initialized\n");
}

void kernel_main() {
    clear_screen();
    print_message("Welcome to Kyro OS!\nYour in control!\n");
    
    kernel_init();
    
    // Enable interrupts
    asm volatile("sti");
    
    // Start login process
    login_prompt();
    
    // Start shell in user mode (create as separate process)
    create_process("shell", (void*)run_shell, false);
    
    // Kernel idle loop
    while (1) {
        schedule(); // Let other processes run
        asm volatile("hlt"); // Halt until next interrupt
    }
}
