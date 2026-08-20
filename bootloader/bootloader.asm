; bootloader.asm
; A Simple Bootloader

org 0x7C00  ; Bootloader is loaded at memory address 0x7C00
bits 16     ; Set the processor to 16-bit mode (real mode)
start: 
    jmp boot

; %include "movcursor.asm" ; Include the movcursor.asm file

;; constant and variable definitions (0ah = line feed, 0dh = carriage return, 0h = null terminator)
msg db "Welcome to My Operating System!", 0ah, 0dh, 0h

boot:
    cli ; no interrupts
    cld ; all that we need to init

    ; Set the stack pointer to 0x7C00
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Set the buffer address to 0x500
    mov ax, 0x50

    ; Set the buffer
    mov es, ax
    xor bx, bx

    mov al, 15 ; read 15 sectors
    mov ch, 0 ; track 0
    mov cl, 2 ; sector to read (2)
    mov dh, 0 ; head number (0)

    mov ah, 0x02 ; BIOS read sector function
    int 0x13 ; call BIOS interrupt
    jmp 0x50:0x0 ; jump and execute the sector

disk_error:
    hlt ; halt the system

; We have to be 512 bits. Clear the rest of the bytes with 0
times 510 - ($ - $$) db 0 ; Fill the rest of the bootloader with zeros
dw 0xAA55 ; Boot signature (must be 0xAA55 for BIOS to recognize it as a valid bootloader)