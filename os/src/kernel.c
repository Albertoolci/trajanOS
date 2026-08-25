// kernel.c
#include "../include/drivers/ports.h"
#include "../include/drivers/screen.h"
#include "../include/cpu/isr.h"

void kernel_main() {
    clear_screen();

    kprintf("TrajanOS Booted Successfully\n\n");
    kprintf("Kernel Base: %x\n", 0x0500);
    kprintf("Video RAM:   %x\n\n", VIDEO_ADDRESS);

    kprintf("Initializing Interrupts (ISRs)...\n");
    isr_install();
    kprintf("IDT Loaded Successfully!\n\n");

    kprintf("Testing ISR 6 (Invalid Opcode)...\n");
    __asm__ volatile("ud2");
    
    // If ever reached, it means the CPU did not catch the exception, which is an error.
    kprintf("ERROR: CPU did not catch the exception!\n");

    
    

    while(1);
}