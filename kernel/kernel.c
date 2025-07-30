#include "kernel.h"
#include "interrupts/idt.h"
#include "memory/paging.h"
#include "timer.h"
#include "syscall.h"
#include "process.h"
#include "io.h"
#include "pic.h"
#include "fs/fs.h"
#include "drivers/keyboard.h"
#include "drivers/mouse.h" 
#include "drivers/network.h"
#include "terminal.h"
#include "user.h"
#include "shell.h"
#include "package_manager.h"
#include "scripting.h"
#include "login.h"
#include "run_shell.h"
#include "run_terminal.h"

// Global video memory pointer and cursor position
static char *video_memory = (char *)0xB8000;
static int cursor_x = 0;
static int cursor_y = 0;
static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;

void clear_screen() {
    char *video_memory = (char *)0xb8000;
    for (int i = 0; i < 80 * 25; i++) {
        video_memory[i * 2] = ' ';
        video_memory[i * 2 + 1] = 0x07;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void print_message(const char *message) {
    while (*message) {
        if (*message == '\n') {
            cursor_x = 0;
            cursor_y++;
        } else if (*message == '\r') {
            cursor_x = 0;
        } else {
            video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2] = *message;
            video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2 + 1] = 0x07;
            cursor_x++;
        }
        
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
        
        if (cursor_y >= VGA_HEIGHT) {
            // Scroll screen up
            for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
                video_memory[i * 2] = video_memory[(i + VGA_WIDTH) * 2];
                video_memory[i * 2 + 1] = video_memory[(i + VGA_WIDTH) * 2 + 1];
            }
            
            // Clear last line
            for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
                video_memory[i * 2] = ' ';
                video_memory[i * 2 + 1] = 0x07;
            }
            
            cursor_y = VGA_HEIGHT - 1;
        }
        
        message++;
    }
}

void putchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\t') {
        cursor_x = (cursor_x + 8) & ~7;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2] = ' ';
        }
    } else {
        video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2] = c;
        video_memory[(cursor_y * VGA_WIDTH + cursor_x) * 2 + 1] = 0x07;
        cursor_x++;
    }
    
    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }
    
    if (cursor_y >= VGA_HEIGHT) {
        // Scroll screen up
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
            video_memory[i * 2] = video_memory[(i + VGA_WIDTH) * 2];
            video_memory[i * 2 + 1] = video_memory[(i + VGA_WIDTH) * 2 + 1];
        }
        
        // Clear last line
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
            video_memory[i * 2] = ' ';
            video_memory[i * 2 + 1] = 0x07;
        }
        
        cursor_y = VGA_HEIGHT - 1;
    }
}

void kernel_init() {
    print_message("Kyro OS - Initializing core systems...\n");
    
    // Initialize core systems first
    print_message("Setting up IDT...\n");
    idt_init();
    
    print_message("Setting up PIC...\n");
    pic_init();
    
    print_message("Setting up timer...\n");
    timer_init(100);   // 100Hz timer
    
    print_message("Setting up memory management...\n");
    paging_init();
    
    print_message("Setting up system calls...\n");
    syscall_init();
    
    print_message("Setting up process management...\n");
    process_init();
    
    // Enable interrupts for timer and keyboard
    print_message("Enabling hardware interrupts...\n");
    pic_enable_irq(0); // Timer
    pic_enable_irq(1); // Keyboard
    
    // Initialize subsystems
    print_message("Initializing subsystems...\n");
    fs_init();
    terminal_init();
    user_init();
    shell_init();
    package_manager_init();
    scripting_init();
    
    // Initialize drivers
    print_message("Initializing drivers...\n");
    keyboard_init();
    mouse_init(); 
    network_init();
    
    // Initialize user system and filesystem
    print_message("Setting up initial data...\n");
    init_users();
    init_file_system();
    
    print_message("Kyro OS - All systems initialized successfully!\n");
}

void kernel_main() { 
    clear_screen();
    print_message("Welcome to Kyro OS!\n");
    print_message("You're in control!\n\n");
    
    // Initialize all kernel systems
    kernel_init();
    
    // Enable interrupts
    print_message("Enabling interrupts...\n");
    asm volatile("sti");

    // Start login process
    print_message("Starting login system...\n");
    login_prompt();
    
    // Create shell process
    print_message("Starting shell...\n");
    create_process("shell", (void*)run_shell, false);
    
    // Enter idle loop
    print_message("Entering system idle loop...\n");
    while (1) {
        asm volatile("hlt"); // Halt until next interrupt
    }
}