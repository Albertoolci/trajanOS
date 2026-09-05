// vmm.h
#ifndef VMM_H
#define VMM_H

#include <stdint.h>

// Virtual base address of the higher-half kernel. See KERNEL_VIRTUAL_BASE in os/linker.ld and os/src/kernel_entry.asm
#define KERNEL_VIRTUAL_BASE 0xC0000000

// Index into a 1024-entry page directory that KERNEL_VIRTUAL_BASE falls into (0xC0000000 >> 22 = 768).
#define KERNEL_PAGE_DIR_INDEX (KERNEL_VIRTUAL_BASE >> 22)

void vmm_init();

#endif