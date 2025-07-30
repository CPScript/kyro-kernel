#include "drivers/keyboard.h"
#include "kernel.h"
#include "io.h"

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
    
    // Handle key release (ignore for now)
    if (scancode & 0x80) {
        scancode &= 0x7F; // Remove release flag
        // Handle key release events here if needed
        return;
    }
    
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
    
    static char scancode_map_shift[] = {
        0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
        '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
        0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
        0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
        '*', 0, ' '
    };
    
    // Handle shift key press/release
    if (scancode == KEY_SHIFT_L || scancode == KEY_SHIFT_R) {
        keyboard_state.flags |= KEYBOARD_FLAG_SHIFT;
        return 0;
    }
    
    if (scancode < sizeof(scancode_map)) {
        if (shift) {
            return scancode_map_shift[scancode];
        } else {
            return scancode_map[scancode];
        }
    }
    
    return 0;
}

// Additional required functions
void keyboard_handler(void) {
    keyboard_interrupt_handler();
}

void keyboard_wait_input(void) {
    while (inb(KEYBOARD_STATUS_PORT) & KEYBOARD_STATUS_INPUT_FULL);
}

void keyboard_wait_output(void) {
    while (!(inb(KEYBOARD_STATUS_PORT) & KEYBOARD_STATUS_OUTPUT_FULL));
}

void keyboard_set_leds(uint8_t leds) {
    keyboard_write_data(KEYBOARD_CMD_SET_LEDS);
    keyboard_write_data(leds);
    keyboard_state.led_state = leds;
}

void keyboard_enable(void) {
    keyboard_write_data(KEYBOARD_CMD_ENABLE);
}

void keyboard_disable(void) {
    keyboard_write_data(KEYBOARD_CMD_DISABLE);
}

bool keyboard_buffer_empty(void) {
    return keyboard_state.buffer_count == 0;
}

bool keyboard_buffer_full(void) {
    return keyboard_state.buffer_count >= KEYBOARD_BUFFER_SIZE;
}

void keyboard_buffer_put(uint8_t scancode) {
    if (!keyboard_buffer_full()) {
        keyboard_state.buffer[keyboard_state.buffer_head] = scancode;
        keyboard_state.buffer_head = (keyboard_state.buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
        keyboard_state.buffer_count++;
    }
}

uint8_t keyboard_buffer_get(void) {
    if (keyboard_buffer_empty()) {
        return 0;
    }
    
    uint8_t scancode = keyboard_state.buffer[keyboard_state.buffer_tail];
    keyboard_state.buffer_tail = (keyboard_state.buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    keyboard_state.buffer_count--;
    return scancode;
}

void keyboard_update_leds(void) {
    keyboard_set_leds(keyboard_state.led_state);
}