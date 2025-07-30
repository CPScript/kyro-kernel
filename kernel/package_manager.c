#include "package_manager.h"
#include <stdio.h>

void add_package(const char *name, const char *version) {
    printf("Package added: %s %s\n", name, version);
}

void install_package(const char *name) {
    printf("Package installed: %s\n", name);
}

void update_package(const char *name) {
    printf("Package updated: %s\n", name);
}

void remove_package(const char *name) {
    printf("Package removed: %s\n", name);
}

void package_manager_init() {
    printf("Package manager initialized.\n");
}
