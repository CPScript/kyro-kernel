#ifndef DRIVERS_KEYBOARD_H
#define DRIVERS_KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

// Keyboard I/O ports
#define KEYBOARD_DATA_PORT    0x60
#define KEYBOARD_STATUS_PORT  0x64
#define KEYBOARD_COMMAND_PORT 0x64

// Keyboard status register bits
#define KEYBOARD_STATUS_OUTPUT_FULL  0x01
#define KEYBOARD_STATUS_INPUT_FULL   0x02
#define KEYBOARD_STATUS_SYSTEM_FLAG  0x04
#define KEYBOARD_STATUS_COMMAND_DATA 0x08
#define KEYBOARD_STATUS_LOCKED       0x10
#define KEYBOARD_STATUS_AUX_DATA     0x20
#define KEYBOARD_STATUS_TIMEOUT      0x40
#define KEYBOARD_STATUS_PARITY_ERROR 0x80

// Keyboard commands
#define KEYBOARD_CMD_SET_LEDS        0xED
#define KEYBOARD_CMD_ECHO            0xEE
#define KEYBOARD_CMD_GET_SET_SCANCODE 0xF0
#define KEYBOARD_CMD_IDENTIFY        0xF2
#define KEYBOARD_CMD_SET_TYPEMATIC   0xF3
#define KEYBOARD_CMD_ENABLE          0xF4
#define KEYBOARD_CMD_DISABLE         0xF5
#define KEYBOARD_CMD_RESET           0xFF

// Keyboard controller commands
#define KEYBOARD_CTRL_READ_CONFIG    0x20
#define KEYBOARD_CTRL_WRITE_CONFIG   0x60
#define KEYBOARD_CTRL_DISABLE_AUX    0xA7
#define KEYBOARD_CTRL_ENABLE_AUX     0xA8
#define KEYBOARD_CTRL_TEST_AUX       0xA9
#define KEYBOARD_CTRL_SELF_TEST      0xAA
#define KEYBOARD_CTRL_TEST_FIRST     0xAB
#define KEYBOARD_CTRL_DISABLE_FIRST  0xAD
#define KEYBOARD_CTRL_ENABLE_FIRST   0xAE

// Special key codes
#define KEY_ESCAPE     0x01
#define KEY_BACKSPACE  0x0E
#define KEY_TAB        0x0F
#define KEY_ENTER      0x1C
#define KEY_CTRL       0x1D
#define KEY_SHIFT_L    0x2A
#define KEY_SHIFT_R    0x36
#define KEY_ALT        0x38
#define KEY_SPACE      0x39
#define KEY_CAPS_LOCK  0x3A
#define KEY_F1         0x3B
#define KEY_F2         0x3C
#define KEY_F3         0x3D
#define KEY_F4         0x3E
#define KEY_F5         0x3F
#define KEY_F6         0x40
#define KEY_F7         0x41
#define KEY_F8         0x42
#define KEY_F9         0x43
#define KEY_F10        0x44
#define KEY_NUM_LOCK   0x45
#define KEY_SCROLL_LOCK 0x46
#define KEY_HOME       0x47
#define KEY_UP         0x48
#define KEY_PAGE_UP    0x49
#define KEY_LEFT       0x4B
#define KEY_RIGHT      0x4D
#define KEY_END        0x4F
#define KEY_DOWN       0x50
#define KEY_PAGE_DOWN  0x51
#define KEY_INSERT     0x52
#define KEY_DELETE     0x53

// Keyboard state flags
#define KEYBOARD_FLAG_SHIFT     0x01
#define KEYBOARD_FLAG_CTRL      0x02
#define KEYBOARD_FLAG_ALT       0x04
#define KEYBOARD_FLAG_CAPS_LOCK 0x08
#define KEYBOARD_FLAG_NUM_LOCK  0x10
#define KEYBOARD_FLAG_SCROLL_LOCK 0x20

// Keyboard buffer size
#define KEYBOARD_BUFFER_SIZE 256

// Keyboard state structure
typedef struct {
    uint8_t flags;
    uint8_t led_state;
    bool extended_scancode;
    uint8_t buffer[KEYBOARD_BUFFER_SIZE];
    int buffer_head;
    int buffer_tail;
    int buffer_count;
} keyboard_state_t;

// Function prototypes
void keyboard_init(void);
void keyboard_handler(void);
uint8_t keyboard_read_data(void);
void keyboard_write_data(uint8_t data);
uint8_t keyboard_read_status(void);
void keyboard_write_command(uint8_t command);
bool keyboard_data_available(void);
char keyboard_get_char(void);
void keyboard_wait_input(void);
void keyboard_wait_output(void);
void keyboard_set_leds(uint8_t leds);
void keyboard_enable(void);
void keyboard_disable(void);
bool keyboard_buffer_empty(void);
bool keyboard_buffer_full(void);
void keyboard_buffer_put(uint8_t scancode);
uint8_t keyboard_buffer_get(void);
char scancode_to_ascii(uint8_t scancode, bool shift);
void keyboard_update_leds(void);

// Interrupt handler
void keyboard_interrupt_handler(void);

// Global keyboard state
extern keyboard_state_t keyboard_state;

#endif // DRIVERS_KEYBOARD_H