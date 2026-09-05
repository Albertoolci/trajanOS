; kernel_entry.asm
;
; Kernel Entry Point.
;
; Responsibilities, in order:
;   1. (Real mode)  Enable the A20 line, load the GDT, switch to 32-bit
;                    Protected Mode.
;   2. (Protected mode, still running at the kernel's low PHYSICAL load
;      address) Build a temporary page directory that maps the first 16MB
;      of physical RAM TWICE: once identity-mapped (0 -> 0), and once at
;      KERNEL_VIRTUAL_BASE (0xC0000000 -> 0), enable PSE + paging.
;   3. Jump into the higher half: from this point on, EIP (and every other
;      address used by the kernel) is a virtual address >= KERNEL_VIRTUAL_BASE.
;   4. Switch to the definitive kernel stack (reserved in .bss, therefore
;      already living in the higher half) and call kernel_main().
;
; The identity mapping of the first 16MB is kept alive permanently by
; vmm_init() (see os/src/mem/vmm.c) so that things that still use raw
; physical addresses -- the VGA text buffer (0xB8000), the E820 map left
; by the bootloader (0x8000), etc. -- keep working unmodified.

%define KERNEL_VIRTUAL_BASE   0xC0000000            ; Must match os/linker.ld and os/include/mem/vmm.h
%define KERNEL_PAGE_DIR_INDEX (KERNEL_VIRTUAL_BASE >> 22) ; = 768 (index into the page directory)

[bits 16] ; Set the processor to 16-bit mode (real mode)

; ---------------------------------------------------------------------
; Boot trampoline. Placed in its own section (.text.boot) which the
; linker script keeps at VMA == LMA == physical load address, because
; this code runs BEFORE paging exists (virtual == physical for it).
; ---------------------------------------------------------------------
section .text.boot progbits alloc exec nowrite align=16

global entry
entry:
    cli ; Disable interrupts

    in al, 0x92 ; Activates the A20 line
    or al, 2
    out 0x92, al ; Reads input/output port 0x92 and sets bit 1 to enable the A20 line

    lgdt [gdt_descriptor] ; Load the GDT descriptor, tells the CPU where the GDT is

    mov eax, cr0
    or eax, 1 ; Set the PE (Protection Enable) bit in CR0 to enable protected mode
    mov cr0, eax

    jmp 0x08:protected_mode_entry ; Far jump to the protected mode entry point (also flushes the prefetch queue)

[bits 32] ; Set the processor to 32-bit mode (protected mode)
protected_mode_entry:
    mov ax, 0x10 ; Load the data segment selector (0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ; ESP still holds whatever the bootloader set it to (0x7C00); it is
    ; not used for anything below (no push/pop/call), and we replace it
    ; with the definitive higher-half stack right after the jump below.

    ; Build a temporary page directory: identity map the first 16MB
    ; of physical RAM (entries 0-3) AND map that same physical range
    ; at KERNEL_VIRTUAL_BASE (entries 768-771), using 4MB PSE pages
    ; so a single page directory (no page tables) is enough.
    mov edi, boot_page_directory
    mov ecx, 1024
    mov eax, 0x00000002        ; Supervisor, Read/Write, Not Present (bit 0 = 0)
.clear_pd:
    mov [edi], eax
    add edi, 4
    loop .clear_pd

    ; Identity mapping: virtual 0x00000000-0x00FFFFFF -> physical 0x00000000-0x00FFFFFF
    mov dword [boot_page_directory + 0*4], 0x00000083 ; 0x000000 | Present, RW, PS(4MB)
    mov dword [boot_page_directory + 1*4], 0x00400083 ; 0x400000
    mov dword [boot_page_directory + 2*4], 0x00800083 ; 0x800000
    mov dword [boot_page_directory + 3*4], 0x00C00083 ; 0xC00000

    ; Higher-half mapping: virtual 0xC0000000-0xC0FFFFFF -> physical 0x00000000-0x00FFFFFF
    mov dword [boot_page_directory + (KERNEL_PAGE_DIR_INDEX+0)*4], 0x00000083
    mov dword [boot_page_directory + (KERNEL_PAGE_DIR_INDEX+1)*4], 0x00400083
    mov dword [boot_page_directory + (KERNEL_PAGE_DIR_INDEX+2)*4], 0x00800083
    mov dword [boot_page_directory + (KERNEL_PAGE_DIR_INDEX+3)*4], 0x00C00083

    ; Enable PSE (Page Size Extension) by setting the PSE bit (Bit 4) in CR4
    mov eax, cr4
    or eax, 0x10
    mov cr4, eax

    ; Load the (physical address of the) temporary page directory into CR3.
    ; NOTE: paging is not enabled yet, so 'boot_page_directory' here still
    ; resolves to its physical address (VMA == LMA in this section).
    mov eax, boot_page_directory
    mov cr3, eax

    ; Enable paging by setting the PG bit (Bit 31) in CR0
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; We are still executing from the low, identity-mapped copy of this
    ; code (EIP == its own physical/VMA address, since that region is
    ; identity mapped). A plain relative JMP to a higher-half label is
    ; computed as (target_VMA - next_EIP); since EIP right now equals this
    ; instruction's own VMA, adding that relative displacement lands EIP
    ; exactly on 'higher_half_entry's higher-half VMA -- which the CPU can
    ; now fetch correctly because we've also mapped that virtual range to
    ; the same physical frames above.
    jmp higher_half_entry

; GDT and the temporary page directory must be reachable (and correct)
; BEFORE paging is enabled, so they stay in the low, identity-mapped
; boot section together with the 16-/32-bit trampoline.

; GDT (Global Descriptor Table) definition
gdt_start:
gdt_null:
    dd 0x0, 0x0 ; First 8 bytes must be null because of Intel's requirement
gdt_code: ; Code Segment Descriptor (Offset 0x08)
    dw 0xffff, 0x0      ; Limit (bits 0-15) = 0xFFFF | Base address (bits 0-15) = 0x0000
    db 0x0              ; Base address (bits 16-23) = 0x00
    db 10011010b        ; Access byte: Present=1, Ring=0 (00=Kernel Level), System=1, Executable=1, Conforming=0, Readable=1, Accessed=0
    db 11001111b        ; Flags + Limit (16-19): Granularity=1 (4KB units), Size=1 (32-bit), Limit bits=1111b (0xF)
    db 0x0              ; Base address (bits 24-31) = 0x00

gdt_data: ; Data Segment Descriptor (Offset 0x10)
    dw 0xffff, 0x0      ; Limit (bits 0-15) = 0xFFFF | Base address (bits 0-15) = 0x0000
    db 0x0              ; Base address (bits 16-23) = 0x00
    db 10010010b        ; Access byte: Present=1, Ring=0 (00=Kernel Level), System=1, Executable=0 (Data), Direction=0, Writable=1, Accessed=0
    db 11001111b        ; Flags + Limit (16-19): Granularity=1 (4KB units), Size=1 (32-bit), Limit bits=1111b (0xF)
    db 0x0              ; Base address (bits 24-31) = 0x00

gdt_end: ; Label used to calculate the total size of the GDT

gdt_descriptor: ; GDT Pointer Structure for the 'lgdt' Instruction
    dw gdt_end - gdt_start - 1  ; GDT Limit (16 bits): Size of GDT in bytes minus 1
    dd gdt_start                ; GDT Base (32 bits): Linear starting address of gdt_start

; Temporary page directory used only during the early boot transition into
; the higher half. Must be 4KB aligned. Placed in .data.boot (still part
; of the low, identity-mapped boot section, see os/linker.ld).
section .data.boot progbits alloc noexec write
boot_page_directory:
    resb 4096

; From this point on, everything is linked (and executes) at
; KERNEL_VIRTUAL_BASE and above: this is the actual higher-half kernel.
section .text
higher_half_entry:
    ; Switch from the temporary low stack to the definitive kernel stack.
    ; 'stack_top' lives in .bss, which the linker places in the higher
    ; half, so this is already a proper higher-half virtual address.
    mov esp, stack_top
    mov ebp, esp

    extern kernel_main
    call kernel_main ; Call the kernel's main function

entry_error:
    hlt ; If we reach this point, something went wrong. Halt the CPU.
    jmp entry_error

; Kernel stack (16 KB), reserved in .bss (higher half).
section .bss
align 16
stack_bottom:
    resb 16384
stack_top: