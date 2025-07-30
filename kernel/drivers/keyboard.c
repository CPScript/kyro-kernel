#include "drivers/keyboard.h"
#include "io.h"
#include "pic.h"
#include "kernel.h"

// Global keyboard state
keyboard_state_t keyboard_state;

// Scancode to ASCII conversion table (US QWERTY)
static char scancode_table[128] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

// Shifted characters
static char scancode_table_shift[128] = {
    0,   27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,   'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,   '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void keyboard_init(void) {
    // Initialize keyboard state
    keyboard_state.flags = 0;
    keyboard_state.led_state = 0;
    keyboard_state.extended_scancode = false;
    keyboard_state.buffer_head = 0;
    keyboard_state.buffer_tail = 0;
    keyboard_state.buffer_count = 0;
    
    // Clear keyboard buffer
    for (int i = 0; i < KEYBOARD_BUFFER_SIZE; i++) {
        keyboard_state.buffer[i] = 0;
    }
    
    print_message("Keyboard driver initialized\n");
}

uint8_t keyboard_read_data(void) {
    return inb(KEYBOARD_DATA_PORT);
}

void keyboard_write_data(uint8_t data) {
    keyboard_wait_input();
    outb(KEYBOARD_DATA_PORT, data);
}

uint8_t keyboard_read_status(void) {
    return inb(KEYBOARD_STATUS_PORT);
}

void keyboard_write_command(uint8_t command) {
    keyboard_wait_input();
    outb(KEYBOARD_COMMAND_PORT, command);
}

void keyboard_wait_input(void) {
    while (keyboard_read_status() & KEYBOARD_STATUS_INPUT_FULL);
}

void keyboard_wait_output(void) {
    while (!(keyboard_read_status() & KEYBOARD_STATUS_OUTPUT_FULL));
}

bool keyboard_data_available(void) {
    return keyboard_state.buffer_count > 0;
}

void keyboard_buffer_put(uint8_t scancode) {
    if (keyboard_state.buffer_count < KEYBOARD_BUFFER_SIZE) {
        keyboard_state.buffer[keyboard_state.buffer_head] = scancode;
        keyboard_state.buffer_head = (keyboard_state.buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
        keyboard_state.buffer_count++;
    }
}

uint8_t keyboard_buffer_get(void) {
    if (keyboard_state.buffer_count > 0) {
        uint8_t scancode = keyboard_state.buffer[keyboard_state.buffer_tail];
        keyboard_state.buffer_tail = (keyboard_state.buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
        keyboard_state.buffer_count--;
        return scancode;
    }
    return 0;
}

bool keyboard_buffer_empty(void) {
    return keyboard_state.buffer_count == 0;
}

bool keyboard_buffer_full(void) {
    return keyboard_state.buffer_count >= KEYBOARD_BUFFER_SIZE;
}

char scancode_to_ascii(uint8_t scancode, bool shift) {
    if (scancode >= 128) return 0;
    
    if (shift) {
        return scancode_table_shift[scancode];
    } else {
        return scancode_table[scancode];
    }
}

void keyboard_handler(void) {
    uint8_t scancode = keyboard_read_data();
    
    // Handle extended scancodes (0xE0 prefix)
    if (scancode == 0xE0) {
        keyboard_state.extended_scancode = true;
        return;
    }
    
    // Check if this is a key release (bit 7 set)
    bool key_released = (scancode & 0x80) != 0;
    scancode &= 0x7F; // Remove release bit
    
    // Handle special keys
    switch (scancode) {
        case KEY_SHIFT_L:
        case KEY_SHIFT_R:
            if (key_released) {
                keyboard_state.flags &= ~KEYBOARD_FLAG_SHIFT;
            } else {
                keyboard_state.flags |= KEYBOARD_FLAG_SHIFT;
            }
            break;
            
        case KEY_CTRL:
            if (key_released) {
                keyboard_state.flags &= ~KEYBOARD_FLAG_CTRL;
            } else {
                keyboard_state.flags |= KEYBOARD_FLAG_CTRL;
            }
            break;
            
        case KEY_ALT:
            if (key_released) {
                keyboard_state.flags &= ~KEYBOARD_FLAG_ALT;
            } else {
                keyboard_state.flags |= KEYBOARD_FLAG_ALT;
            }
            break;
            
        case KEY_CAPS_LOCK:
            if (!key_released) {
                keyboard_state.flags ^= KEYBOARD_FLAG_CAPS_LOCK;
                keyboard_update_leds();
            }
            break;
            
        case KEY_NUM_LOCK:
            if (!key_released) {
                keyboard_state.flags ^= KEYBOARD_FLAG_NUM_LOCK;
                keyboard_update_leds();
            }
            break;
            
        case KEY_SCROLL_LOCK:
            if (!key_released) {
                keyboard_state.flags ^= KEYBOARD_FLAG_SCROLL_LOCK;
                keyboard_update_leds();
            }
            break;
            
        default:
            // Regular key - only process key press, not release
            if (!key_released) {
                keyboard_buffer_put(scancode);
            }
            break;
    }
    
    keyboard_state.extended_scancode = false;
}

char keyboard_get_char(void) {
    if (keyboard_buffer_empty()) {
        return 0;
    }
    
    uint8_t scancode = keyboard_buffer_get();
    bool shift_pressed = (keyboard_state.flags & KEYBOARD_FLAG_SHIFT) != 0;
    bool caps_lock = (keyboard_state.flags & KEYBOARD_FLAG_CAPS_LOCK) != 0;
    
    // For letters, caps lock acts like shift
    char c = scancode_to_ascii(scancode, shift_pressed);
    if (c >= 'a' && c <= 'z' && caps_lock) {
        c = c - 'a' + 'A';
    } else if (c >= 'A' && c <= 'Z' && caps_lock && !shift_pressed) {
        c = c - 'A' + 'a';
    }
    
    return c;
}

void keyboard_set_leds(uint8_t leds) {
    keyboard_write_data(KEYBOARD_CMD_SET_LEDS);
    keyboard_write_data(leds);
    keyboard_state.led_state = leds;
}

void keyboard_update_leds(void) {
    uint8_t leds = 0;
    if (keyboard_state.flags & KEYBOARD_FLAG_SCROLL_LOCK) leds |= 0x01;
    if (keyboard_state.flags & KEYBOARD_FLAG_NUM_LOCK) leds |= 0x02;
    if (keyboard_state.flags & KEYBOARD_FLAG_CAPS_LOCK) leds |= 0x04;
    
    keyboard_set_leds(leds);
}

void keyboard_enable(void) {
    keyboard_write_data(KEYBOARD_CMD_ENABLE);
}

void keyboard_disable(void) {
    keyboard_write_data(KEYBOARD_CMD_DISABLE);
}

// Interrupt handler called from assembly
void keyboard_interrupt_handler(void) {
    keyboard_handler();
    pic_send_eoi(IRQ_KEYBOARD);
}