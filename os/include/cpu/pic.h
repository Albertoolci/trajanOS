// pic.h - Header file for PIC (Programmable Interrupt Controller) functions
#ifndef PIC_H
#define PIC_H

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

#define PIC_EOI      0x20

#include <stdint.h>

void pic_remap();
void pic_send_eoi(uint8_t irq);

#endif