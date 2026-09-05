// idt.h
#ifndef IDT_H
#define IDT_H

#define IDT_ENTRIES 256

#include <stdint.h>

// Structure of an Interrupt Descriptor Table (IDT) entry (8 bytes)
typedef struct {
    uint16_t low_offset;  // Bits 0..15 of the address to jump to when this interrupt fires
    uint16_t sel;         // Kernel segment selector
    uint8_t  always0;     // Always 0, unused byte
    uint8_t  flags;       // Attributes and level of privilege
    uint16_t high_offset; // Bits 16..31 of the address to jump to when this interrupt fires
} __attribute__((packed)) idt_entry_t;

// Structure of the pointer to the IDT (used by the lidt instruction)
typedef struct {
    uint16_t limit; // Total length of the IDT in bytes - 1
    uint32_t base;  // Address of the first entry in the IDT (&idt[0])
} __attribute__((packed)) idt_ptr_t;

void set_idt_gate(int n, unsigned int handler);
void init_idt();
void clean_idt();
void load_idt();

#endif