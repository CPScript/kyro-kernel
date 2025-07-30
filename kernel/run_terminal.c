#include "run_terminal.h"
#include "kernel.h"

void run_terminal() {
    print_message("Terminal mode activated.\n");
    print_message("This is a basic terminal interface.\n");
    
    // For now, just run the shell if possible
    run_shell();
}