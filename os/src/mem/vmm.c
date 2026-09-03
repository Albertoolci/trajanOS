#include "../../include/mem/vmm.h"
#include "../../include/drivers/screen.h"

// Page Directory must be 4KB aligned (4096 bytes)
__attribute__((aligned(4096))) static uint32_t page_directory[1024];

/**
 * @brief Initializes the Virtual Memory Manager (VMM) and enables paging with PSE.
 * @details Sets up a Page Directory using 4MB PSE (Page Size Extension) pages to identity-map the first 16MB of physical memory (covering kernel, VGA text buffer, and stack at 0x900000), enables PSE in CR4, loads the page directory into CR3, and sets the PG bit in CR0.
 * @return None
 */
void vmm_init() {
    // Initialize the Page Directory with 1024 entries, all set to not present (0x00000002)
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002; // Supervisor, Read/Write, Not Present (bit 0 = 0)
    }

    // Identity map the first 16MB of physical memory using 4MB PSE pages
    for (int i = 0; i < 4; i++) {
        page_directory[i] = (i * 0x400000) | 0x00000083; 
    }

    // Enable PSE (Page Size Extension) by setting the PSE bit (Bit 4) in CR4 register
    __asm__ volatile (
        "mov %%cr4, %%eax\n\t"
        "or $0x10, %%eax\n\t"     // Set PSE bit (4)
        "mov %%eax, %%cr4"
        ::: "eax"
    );

    // Load the Page Directory into CR3 register
    __asm__ volatile (
        "mov %0, %%eax\n\t"
        "mov %%eax, %%cr3"
        : : "r" ((uint32_t)page_directory)
        : "eax"
    );

    // Enable paging by setting the PG bit (Bit 31) in CR0 register
    __asm__ volatile (
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t" // Set PG bit (31)
        "mov %%eax, %%cr0"
        ::: "eax"
    );

    kprintf("VMM Initialized: Paging enabled with 4MB PSE pages (0-16MB identity mapped).\n");
}