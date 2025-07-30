#include "timer.h"
#include "io.h"

static volatile uint32_t tick_count = 0;

void timer_init(uint32_t frequency) {
    // Calculate divisor for desired frequency
    uint32_t divisor = PIT_FREQUENCY / frequency;
    
    // Send command byte
    outb(0x43, 0x36);
    
    // Send frequency divisor
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    
    outb(0x40, low);
    outb(0x40, high);
}

void timer_handler(void) {
    tick_count++;
    
    // Schedule next task if multitasking is enabled
    // schedule_next_task();
    
    // Send EOI to PIC
    outb(0x20, 0x20);
}

uint32_t get_tick_count(void) {
    return tick_count;
}

void sleep(uint32_t milliseconds) {
    uint32_t start_ticks = tick_count;
    uint32_t target_ticks = start_ticks + (milliseconds / 10); // Assuming 100Hz timer
    
    while (tick_count < target_ticks) {
        asm volatile("hlt"); // Halt until next interrupt
    }
}
