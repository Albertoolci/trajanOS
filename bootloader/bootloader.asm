; bootloader.asm
; A Simple Bootloader

org 0x7C00  ; Bootloader is loaded at memory address 0x7C00
bits 16     ; Set the processor to 16-bit mode (real mode)
start: 
    jmp boot

; Constant defining the number of sectors to read from disk. This can be overridden by passing a value from the Makefile.
%ifndef SECTORS_TO_READ
    SECTORS_TO_READ equ 64  ; Value can be overridden by passing a value from the Makefile
%endif

; Standard 1.44 MB floppy disk geometry
SECTORS_PER_TRACK  equ 18
HEADS_PER_CYLINDER equ 2

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

    mov [boot_drive], dl    ; Save the boot drive provided by BIOS

    ; Set the buffer address to 0x0500 (0x0050:0x0000)
    mov ax, 0x0050
    mov es, ax
    xor bx, bx              ; BX always 0 for int 0x13

    mov cx, SECTORS_TO_READ ; Load defined sector count
    mov si, 1               ; SI = Initial LBA sector (physical sector 2 on disk)

read_loop:
    push cx                 ; Preserve remaining sectors counter

    mov ax, si              ; AX = Current LBA
    call lba_to_chs         ; Convert AX to CHS (CL=sector, CH=cylinder, DH=head)

    mov dl, [boot_drive]
    mov ax, 0x0201          ; BIOS read sector function (AH=0x02, AL=1 sector)
    int 0x13                ; call BIOS interrupt
    jc disk_error           ; If disk error, jump to error handler

    ; Advance the ES segment by 512 bytes (0x0020 in segment arithmetic)
    mov ax, es
    add ax, 0x0020
    mov es, ax

    inc si                  ; Next LBA sector
    pop cx                  ; Restore sector counter
    loop read_loop          ; Decrement CX and loop if CX > 0

    sti                     ; Re-enable interrupts before jumping
    jmp 0x0050:0x0000       ; jump and execute the sector

disk_error:
    hlt ; halt the system

; Converts LBA (AX) -> CH (Cylinder), DH (Head), CL (Sector)
lba_to_chs:
    push bx
    
    xor dx, dx
    mov bx, SECTORS_PER_TRACK
    div bx                  ; AX = LBA / 18, DX = LBA % 18

    inc dx                  ; Sectors are 1-based
    mov cl, dl              ; CL = Sector (1-18)

    xor dx, dx
    mov bx, HEADS_PER_CYLINDER
    div bx                  ; AX = Cylinder, DX = Head

    mov ch, al              ; CH = Cylinder
    mov dh, dl              ; DH = Head

    pop bx
    ret

boot_drive db 0

; We have to be 512 bytes. Clear the rest of the bytes with 0
times 510 - ($ - $$) db 0 ; Fill the rest of the bootloader with zeros
dw 0xAA55 ; Boot signature (must be 0xAA55 for BIOS to recognize it as a valid bootloader)