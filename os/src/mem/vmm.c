#include "../../include/mem/vmm.h"
#include "../../include/drivers/screen.h"

// Page Directory must be 4KB aligned (4096 bytes)
__attribute__((aligned(4096))) static unsigned int page_directory[1024];

/**
 * @brief Initializes the Virtual Memory Manager (VMM) and enables kernel paging with PSE.
 * @details Sets up a Page Directory using 4MB PSE (Page Size Extension) pages that maps
 * the first 16MB of physical memory twice: once identity-mapped at 0x00000000 (covering
 * the boot trampoline, the VGA text buffer, the E820 map, and any other code/data that
 * still uses raw physical addresses), and once at KERNEL_VIRTUAL_BASE (0xC0000000), where
 * the higher-half kernel itself is linked to run. This replaces the temporary page
 * directory built by the boot trampoline in kernel_entry.asm with the permanent one used
 * for the rest of the kernel's lifetime. Enables PSE in CR4, loads the page directory into
 * CR3, and (re)sets the PG bit in CR0.
 * @return None
 */
void vmm_init() {
    // Initialize the Page Directory with 1024 entries, all set to not present (0x00000002)
    for (int i = 0; i < 1024; i++) {
        page_directory[i] = 0x00000002; // Supervisor, Read/Write, Not Present (bit 0 = 0)
    }

    // Identity map the first 16MB of physical memory using 4MB PSE pages (virtual 0x00000000-0x00FFFFFF -> physical 0x00000000-0x00FFFFFF)
    for (int i = 0; i < 4; i++) {
        page_directory[i] = (i * 0x400000) | 0x00000083;
    }

    // Map the same first 16MB of physical memory at KERNEL_VIRTUAL_BASE (virtual 0xC0000000-0xC0FFFFFF -> physical 0x00000000-0x00FFFFFF),
    //which is where the higher-half kernel (.text/.rodata/.data/.bss) is actually linked.
    for (int i = 0; i < 4; i++) {
        page_directory[KERNEL_PAGE_DIR_INDEX + i] = (i * 0x400000) | 0x00000083;
    }

    // Enable PSE (Page Size Extension) by setting the PSE bit (Bit 4) in CR4 register
    __asm__ volatile (
        "mov %%cr4, %%eax\n\t"
        "or $0x10, %%eax\n\t"     // Set PSE bit (4)
        "mov %%eax, %%cr4"
        ::: "eax"
    );

    // Load the Page Directory into CR3 register. 'page_directory' is a higher-half virtual symbol, but since we keep the
    // identity mapping alive, its virtual address also happens to be a valid pointer we can compute from; what CR3 actually needs, however, 
    // is its PHYSICAL address, so we subtract KERNEL_VIRTUAL_BASE to recover it.
    __asm__ volatile (
        "mov %0, %%eax\n\t"
        "mov %%eax, %%cr3"
        : : "r" ((unsigned int)page_directory - KERNEL_VIRTUAL_BASE)
        : "eax"
    );

    // Enable paging by setting the PG bit (Bit 31) in CR0 register (paging is already enabled by kernel_entry.asm; 
    // this keeps it enabled now that CR3 points at the permanent, higher-half-aware page directory)
    __asm__ volatile (
        "mov %%cr0, %%eax\n\t"
        "or $0x80000000, %%eax\n\t" // Set PG bit (31)
        "mov %%eax, %%cr0"
        ::: "eax"
    );

    kprintf("VMM Initialized: Paging enabled with 4MB PSE pages (0-16MB identity mapped + 0-16MB mapped at %x, higher-half).\n", KERNEL_VIRTUAL_BASE);
}