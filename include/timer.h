#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define PIT_FREQUENCY 1193180
#define TIMER_IRQ 0

void timer_init(uint32_t frequency);
void timer_handler(void);
uint32_t get_tick_count(void);
void sleep(uint32_t milliseconds);

#endif