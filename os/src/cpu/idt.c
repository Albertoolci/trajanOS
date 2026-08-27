// idt.c
#include "../../include/cpu/idt.h"
#include "../../include/libc/memory.h"

// Global Interrupt Descriptor Table (IDT) and IDT pointer
idt_entry_t idt[IDT_ENTRIES];
idt_ptr_t idt_ptr;

/**
 * @brief Sets an entry in the Interrupt Descriptor Table (IDT).
 * @details This function configures an entry in the IDT at index `n` with the specified interrupt handler address. It sets the low and high offsets of the handler, the kernel code segment selector, and the appropriate flags for the interrupt gate.
 * @param n The index of the IDT entry to set (0-255).
 * @param handler The address of the interrupt handler function.
 * @return None
 */
void set_idt_gate(int n, unsigned int handler) {
    idt[n].low_offset = handler & 0xFFFF;
    idt[n].high_offset = (handler >> 16) & 0xFFFF;
    idt[n].sel = 0x08; // Kernel code segment selector
    idt[n].always0 = 0;
    idt[n].flags = 0x8E; // Present, ring 0    
}

/**
 * @brief Initializes the Interrupt Descriptor Table (IDT).
 * @details This function sets up the IDT by defining its size and base address.
 * @return None
 */
void init_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * IDT_ENTRIES - 1;
    idt_ptr.base = (unsigned int)&idt;
}

/**
 * @brief Cleans the Interrupt Descriptor Table (IDT).
 * @details This function clears all entries in the IDT by setting them to zero.
 * @return None
 */
void clean_idt() {
    memset(&idt, 0, sizeof(idt_entry_t) * IDT_ENTRIES);
}

/**
 * @brief Loads the Interrupt Descriptor Table (IDT) into the CPU.
 * @details This function uses the `lidt` assembly instruction to load the IDT pointer into the CPU's IDT register, enabling the CPU to use the configured IDT for handling interrupts.
 * @return None
 */
void load_idt() {
    __asm__ volatile("lidt %0" :: "m"(idt_ptr));
}