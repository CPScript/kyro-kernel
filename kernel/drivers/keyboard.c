#include "drivers/keyboard.h"
#include "kernel.h"

keyboard_state_t keyboard_state;

void keyboard_init() {
    keyboard_state.flags = 0;
    keyboard_state.led_state = 0;
    keyboard_state.extended_scancode = false;
    keyboard_state.buffer_head = 0;
    keyboard_state.buffer_tail = 0;
    keyboard_state.buffer_count = 0;
    
    printf("Keyboard initialized.\n");
}

void keyboard_interrupt_handler(void) {
    uint8_t scancode = keyboard_read_data();
    
    if (keyboard_state.buffer_count < KEYBOARD_BUFFER_SIZE) {
        keyboard_state.buffer[keyboard_state.buffer_head] = scancode;
        keyboard_state.buffer_head = (keyboard_state.buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
        keyboard_state.buffer_count++;
    }
}

uint8_t keyboard_read_data(void) {
    return inb(KEYBOARD_DATA_PORT);
}

void keyboard_write_data(uint8_t data) {
    while (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_STATUS_INPUT_FULL);
    outb(KEYBOARD_DATA_PORT, data);
}

uint8_t keyboard_read_status(void) {
    return inb(KEYBOARD_STATUS_PORT);
}

void keyboard_write_command(uint8_t command) {
    while (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_STATUS_INPUT_FULL);
    outb(KEYBOARD_COMMAND_PORT, command);
}

bool keyboard_data_available(void) {
    return keyboard_state.buffer_count > 0;
}

char keyboard_get_char(void) {
    if (!keyboard_data_available()) {
        return 0;
    }
    
    uint8_t scancode = keyboard_state.buffer[keyboard_state.buffer_tail];
    keyboard_state.buffer_tail = (keyboard_state.buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    keyboard_state.buffer_count--;
    
    // Convert scancode to ASCII (simplified)
    return scancode_to_ascii(scancode, keyboard_state.flags & KEYBOARD_FLAG_SHIFT);
}

char scancode_to_ascii(uint8_t scancode, bool shift) {
    static char scancode_map[] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*', 0, ' '
    };
    
    if (scancode < sizeof(scancode_map)) {
        char c = scancode_map[scancode];
        if (shift && c >= 'a' && c <= 'z') {
            c = c - 'a' + 'A';
        }
        return c;
    }
    
    return 0;
}
