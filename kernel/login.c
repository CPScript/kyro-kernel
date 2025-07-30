#include "login.h"
#include "kernel.h"
#include "user.h"

void login_prompt() {
    char username[64];
    char password[64];
    
    print_message("\n=== Login ===\n");
    print_message("Username: ");
    
    // Simple input without scanf for now
    terminal_get_line(username, sizeof(username));
    
    print_message("Password: ");
    terminal_get_line(password, sizeof(password));
    
    if (authenticate_user(username, password)) {
        print_message("Login successful! Welcome to Kyro OS.\n");
    } else {
        print_message("Login failed. Using default access.\n");
    }
}