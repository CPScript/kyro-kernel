#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// PIC ports
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

// End of Interrupt command
#define PIC_EOI         0x20

// IRQ numbers
#define IRQ_TIMER       0
#define IRQ_KEYBOARD    1
#define IRQ_CASCADE     2
#define IRQ_COM2        3
#define IRQ_COM1        4
#define IRQ_LPT2        5
#define IRQ_FLOPPY      6
#define IRQ_LPT1        7
#define IRQ_RTC         8
#define IRQ_FREE1       9
#define IRQ_FREE2       10
#define IRQ_FREE3       11
#define IRQ_PS2_MOUSE   12
#define IRQ_FPU         13
#define IRQ_PRIMARY_ATA 14
#define IRQ_SECONDARY_ATA 15

// Function prototypes
void pic_init(void);
void pic_enable_irq(uint8_t irq);
void pic_disable_irq(uint8_t irq);
void pic_send_eoi(uint8_t irq);
void pic_mask_all(void);
void pic_unmask_all(void);
uint16_t pic_get_irr(void);
uint16_t pic_get_isr(void);

#endif
