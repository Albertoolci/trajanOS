// kernel.c
#include "../include/drivers/ports.h"
#include "../include/drivers/screen.h"
#include "../include/cpu/isr.h"
#include "../include/cpu/timer.h"
#include "../include/drivers/keyboard.h"
#include "../include/drivers/shell.h"
#include "../include/mem/pmm.h"

void kernel_main() {
    clear_screen();

    kprintf("TrajanOS Booted Successfully\n\n");
    kprintf("Kernel Base: %x\n", 0x0500);
    kprintf("Video RAM:   %x\n\n", VIDEO_ADDRESS);

    kprintf("Initializing Interrupts (ISRs)...\n");
    interrupts_init();
    kprintf("IDT Loaded Successfully!\n\n");

    kprintf("Initializing Physical Memory Manager (PMM)...\n");
    pmm_init();
    
    unsigned int total_mb = (pmm_get_total_blocks() * PMM_BLOCK_SIZE) / (1024 * 1024);
    unsigned int free_mb = (pmm_get_free_blocks() * PMM_BLOCK_SIZE) / (1024 * 1024);
    unsigned int used_mb = (pmm_get_used_blocks() * PMM_BLOCK_SIZE) / (1024 * 1024);

    kprintf("PMM Initialized:\n");
    kprintf("  Total RAM: %d MB (%d blocks)\n", total_mb, pmm_get_total_blocks());
    kprintf("  Free RAM:  %d MB (%d blocks)\n", free_mb, pmm_get_free_blocks());
    kprintf("  Used RAM:  %d MB (%d blocks)\n\n", used_mb, pmm_get_used_blocks());

    kprintf("Initializing Virtual Memory Manager (VMM)...\n");
    vmm_init();
    kprintf("\n");

    init_keyboard();
    shell_init();
    init_timer(100);
    __asm__ volatile("sti");

    while (1) {
        __asm__ volatile("hlt"); // Pauses the CPU until the next interrupt
    }


    kprintf("Testing ISR 6 (Invalid Opcode)...\n");
    __asm__ volatile("ud2");
    
    // If ever reached, it means the CPU did not catch the exception, which is an error.
    kprintf("ERROR: CPU did not catch the exception!\n");

    
    

    while(1);
}