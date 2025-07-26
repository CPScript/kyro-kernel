#include "networking.h"
#include "kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOR_CHECK "tor_check"
#define FIRST_BOOT_FLAG TOR_CHECK "/first_boot_flag"

int check_tor_installed() {
    // Check if Tor is installed
    int result = system("command -v tor > /dev/null 2>&1");
    return (result == 0); // Returns 1 if installed, 0 if not
}

void install_tor() {
    print_message("Installing Tor...");

    const char *install_command = "apt-get install tor -y"; // This is a Debian-based system

    int result = system(install_command);

    if (result == 0) {
        print_message("Tor installed successfully.");
    } else {
        print_message("Failed to install Tor. Please check your package manager.");
    }
}

int is_first_boot() {
    FILE *file = fopen(FIRST_BOOT_FLAG, "r");
    if (file) {
        fclose(file);
        return 0; // Not the first boot
    }
    return 1; // First boot
}

void create_first_boot_flag() {
    system("mkdir -p " TOR_CHECK);
    FILE *file = fopen(FIRST_BOOT_FLAG, "w");
    if (file) {
        fclose(file);
    } else {
        print_message("Failed to create first boot flag file. Please report issue to developer");
    }
}

void start_tor() {
    if (is_first_boot()) {
        print_message("First boot detected. Installing Tor...");
        install_tor();
        create_first_boot_flag();
    } else {
        print_message("Checking for Tor installation...");
        if (!check_tor_installed()) {
            print_message("Tor is not installed. Installing...");
            install_tor();
        } else {
            print_message("Tor is already installed. Continuing...");
        }
    }

    print_message("Starting Tor in background...");
    int result = system("tor &");

    if (result == 0) {
        print_message("Tor started successfully.");
    } else {
        print_message("Failed to start Tor. Please ensure it is installed and that there are no external scripts preventing its execution.");
    }
}
