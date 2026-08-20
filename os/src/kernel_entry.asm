; kernel_entry.asm
; Kernel Entry Point: Sets protected mode (32-bit mode), loads the GDT and jumps to the kernel's main function.

[bits 16] ; Set the processor to 16-bit mode (real mode)
; [org 0x0500]

global entry:
    cli ; Disable interrupts

    in al, 0x92 ; Activates the A20 line
    or al, 2
    out 0x92, al ; Reads input/output port 0x92 and sets bit 1 to enable the A20 line

    lgdt [gdt_descriptor] ; Load the GDT descriptor, tells the CPU where the GDT

    mov eax, cr0
    or eax, 1 ; Set the PE (Protection Enable) bit in CR0 to enable protected mode
    mov cr0, eax

    jmp 0x08:protected_mode_entry ; Far jump to the protected mode entry point

[bits 32] ; Set the processor to 32-bit mode (protected mode)
protected_mode_entry:
    mov ax, 0x10 ; Load the data segment selector (0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x900000 ; Set the stack pointer to 0x900000, which is a safe location in memory for the stack
                      ; The stack will grow downwards from this address, 
                      ; so we need to ensure that there is enough space allocated for the stack

    extern kernel_main
    call kernel_main ; Call the kernel's main function

entry_error:
    hlt ; If we reach this point, something went wrong. Halt the CPU.

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