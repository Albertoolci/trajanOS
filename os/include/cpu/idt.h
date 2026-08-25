// idt.h
#ifndef IDT_H
#define IDT_H

#define IDT_ENTRIES 256

// Structure of an Interrupt Descriptor Table (IDT) entry (8 bytes)
typedef struct {
    unsigned short low_offset;  // Bits 0..15 of the address to jump to when this interrupt fires
    unsigned short sel;         // Kernel segment selector
    unsigned char  always0;     // Always 0, unused byte
    unsigned char  flags;       // Attributes and level of privilege
    unsigned short high_offset; // Bits 16..31 of the address to jump to when this interrupt fires
} __attribute__((packed)) idt_entry_t;

// Structure of the pointer to the IDT (used by the lidt instruction)
typedef struct {
    unsigned short limit; // Total length of the IDT in bytes - 1
    unsigned int   base;  // Address of the first entry in the IDT (&idt[0])
} __attribute__((packed)) idt_ptr_t;

void set_idt_gate(int n, unsigned int handler);
void init_idt();

#endif