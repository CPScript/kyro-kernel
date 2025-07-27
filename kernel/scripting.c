#include "scripting.h"
#include <stdio.h>

void add_script(const char *script) {
    printf("Script added: %s\n", script);
}

void run_script(const char *script) {
    printf("Script ran: %s\n", script);
}

void scripting_init() {
    printf("Scripting initialized.\n");
}
