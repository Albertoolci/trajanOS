/* linker.ld
 *
 * Higher-Half Kernel linker script.
 *
 * The kernel is still LOADED by the bootloader as a flat binary at a low
 * physical address (KERNEL_PHYSICAL_BASE), because our custom bootloader
 * has no notion of ELF program headers: it just copies raw sectors from
 * disk into memory and jumps to the first loaded byte in real mode.
 *
 * However, once kernel_entry.asm enables paging (see os/src/kernel_entry.asm)
 * the vast majority of the kernel (everything except the small 16-/32-bit
 * boot trampoline) executes using LINEAR addresses starting at
 * KERNEL_VIRTUAL_BASE (0xC0000000, i.e. the classic 3GB "higher half").
 *
 * This is achieved by giving each output section a Virtual Memory Address, 
 * which is KERNEL_VIRTUAL_BASE higher than its Load Memory Address.
 * The AT() directive is what lets VMA and LMA differ. `ld --oformat binary`
 * lays out the final flat binary according to LMA, so the file itself stays
 * small and is placed exactly where the bootloader/kernel_entry.asm expects
 * it physically, while the ELF (kernel.elf, used for GDB symbols) keeps the
 * higher-half VMAs so that debugging works transparently once paging is on.
 */

ENTRY(entry)

KERNEL_VIRTUAL_BASE = 0xC0000000;
KERNEL_PHYSICAL_BASE = 0x00000500;

SECTIONS
{
    /* Boot trampoline: 16-bit real-mode entry, protected-mode switch,
     * temporary page directory and paging enable. This code (and the
     * data it depends on, i.e. the GDT and the temporary page directory)
     * MUST keep VMA == LMA == physical load address, because it runs
     * BEFORE paging exists, when linear addresses are physical addresses.
    */
    . = KERNEL_PHYSICAL_BASE;
    .boot.text : {
        *(.text.boot)
        /* Align only the location counter at THIS point (not the whole
           output section's start address) to 4096 bytes, so that
           boot_page_directory below is page-aligned without forcing
           KERNEL_PHYSICAL_BASE itself to be rounded up. */
        . = ALIGN(4096);
        *(.data.boot)
    }

    /* Higher-half kernel: everything from here on is linked to run at
     * KERNEL_VIRTUAL_BASE + physical_offset, but is still physically
     * stored (LMA) right after the boot trampoline, contiguous in the
     * flat kernel.bin file.
    */
    . += KERNEL_VIRTUAL_BASE;

    .text ALIGN(4096) : AT(ADDR(.text) - KERNEL_VIRTUAL_BASE) {
        *(.text)
    }

    .rodata ALIGN(4096) : AT(ADDR(.rodata) - KERNEL_VIRTUAL_BASE) {
        *(.rodata*)
    }

    /* Data section: Place all initialized global and static variables here */
    .data ALIGN(4096) : AT(ADDR(.data) - KERNEL_VIRTUAL_BASE) {
        *(.data)
    }

    /* BSS section: Place all uninitialized global and static variables here */
    .bss ALIGN(4096) : AT(ADDR(.bss) - KERNEL_VIRTUAL_BASE) {
        *(COMMON)
        *(.bss)
    }

    /* Virtual (higher-half) address of the end of the kernel image. */
    _kernel_end_virtual = .;

    /* Physical address of the end of the kernel image. pmm.c uses this
     * (as a plain integer, then dereferences it through the identity
     * mapping that vmm_init() keeps active) to know where it is safe to
     * place the physical memory bitmap. 
    */
    _kernel_end = . - KERNEL_VIRTUAL_BASE;

    /* Sanity check: the whole kernel image (boot trampoline + higher-half
     * sections) must fit inside the first 4MB PSE page/page-directory
     *  entry that kernel_entry.asm and vmm.c map for both the identity
     * and the higher-half window. Raise this (and add more PSE entries
     * in kernel_entry.asm / vmm.c) if the kernel ever grows past 4MB. 
    */
    ASSERT(_kernel_end <= 0x00400000, "Error: Kernel physical footprint exceeds the 4MB mapped by the boot/VMM page directory!")
}