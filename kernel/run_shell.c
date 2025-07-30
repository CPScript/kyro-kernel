#include "run_shell.h"
#include "kernel.h"
#include "terminal.h"

void run_shell() {
    char command[256];
    
    print_message("\n=== Kyro OS Shell ===\n");
    print_message("Type 'help' for available commands\n");
    
    while (1) {
        print_message("kyro> ");
        terminal_get_line(command, sizeof(command));
        
        if (strcmp(command, "help") == 0) {
            print_message("Available commands:\n");
            print_message("  help    - Show this help\n");
            print_message("  clear   - Clear screen\n");
            print_message("  exit    - Exit shell\n");
            print_message("  ls      - List files\n");
            print_message("  users   - List users\n");
        } else if (strcmp(command, "clear") == 0) {
            clear_screen();
        } else if (strcmp(command, "exit") == 0) {
            print_message("Exiting shell...\n");
            break;
        } else if (strcmp(command, "ls") == 0) {
            list_files();
        } else if (strcmp(command, "users") == 0) {
            list_users();
        } else if (strlen(command) > 0) {
            print_message("Unknown command: ");
            print_message(command);
            print_message("\n");
        }
    }
}