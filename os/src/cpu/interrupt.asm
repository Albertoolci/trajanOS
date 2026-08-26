; interrupt.asm
[bits 32]

; Make isr_handler visible to the linker so that it can be called from C code
extern isr_handler

; Macro for exceptions WITHOUT error code (the CPU does not push err_code, pushes a fake 0)
%macro ISR_NOERRCODE 1
    global isr%1
    isr%1:
        cli                 ; Disable interrupts
        push byte 0         ; Push fake 0 as error code
        push byte %1        ; Push interrupt number
        jmp isr_common_stub
%endmacro

; Macro for exceptions WITH error code (the CPU pushes err_code)
%macro ISR_ERRCODE 1
    global isr%1
    isr%1:
        cli                 ; Disable interrupts
        push byte %1        ; Push interrupt number
        jmp isr_common_stub
%endmacro

%macro IRQ 2
    global irq%1
    irq%1:
        cli
        push byte 0         ; fake error code (0) for IRQs, as they do not have an error code
        push byte %2        ; IDT vector number for the IRQ (32..47)
        jmp irq_common_stub
%endmacro

; IRQ instances for IRQs 0 to 15, mapped to IDT vectors 32 to 47
IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47


ISR_NOERRCODE 0    ; 0: Divide By Zero Exception
ISR_NOERRCODE 1    ; 1: Debug Exception
ISR_NOERRCODE 2    ; 2: Non Maskable Interrupt (NMI)
ISR_NOERRCODE 3    ; 3: Breakpoint Exception
ISR_NOERRCODE 4    ; 4: Into Detected Overflow Exception
ISR_NOERRCODE 5    ; 5: Out of Bounds Exception (BOUND range exceeded)
ISR_NOERRCODE 6    ; 6: Invalid Opcode Exception
ISR_NOERRCODE 7    ; 7: No Coprocessor Exception (Device Not Available)
ISR_ERRCODE   8    ; 8: Double Fault Exception
ISR_NOERRCODE 9    ; 9: Coprocessor Segment Overrun Exception
ISR_ERRCODE   10   ; 10: Bad TSS Exception (Invalid TSS)
ISR_ERRCODE   11   ; 11: Segment Not Present Exception
ISR_ERRCODE   12   ; 12: Stack Fault Exception
ISR_ERRCODE   13   ; 13: General Protection Fault Exception (GPF)
ISR_ERRCODE   14   ; 14: Page Fault Exception
ISR_NOERRCODE 15   ; 15: Unknown Interrupt / Reserved
ISR_NOERRCODE 16   ; 16: x87 FPU Floating-Point Error (Math Fault)
ISR_ERRCODE   17   ; 17: Alignment Check Exception
ISR_NOERRCODE 18   ; 18: Machine Check Exception
ISR_NOERRCODE 19   ; 19: SIMD Floating-Point Exception
ISR_NOERRCODE 20   ; 20: Virtualization Exception
ISR_NOERRCODE 21   ; 21: Control Protection Exception
ISR_NOERRCODE 22   ; 22: Reserved
ISR_NOERRCODE 23   ; 23: Reserved
ISR_NOERRCODE 24   ; 24: Reserved
ISR_NOERRCODE 25   ; 25: Reserved
ISR_NOERRCODE 26   ; 26: Reserved
ISR_NOERRCODE 27   ; 27: Reserved
ISR_NOERRCODE 28   ; 28: Hypervisor Injection Exception
ISR_NOERRCODE 29   ; 29: VMM Communication Exception
ISR_ERRCODE   30   ; 30: Security Exception
ISR_NOERRCODE 31   ; 31: Reserved


; Common handler stub for all ISRs. This code is executed after the CPU pushes the registers and before calling the C handler.
isr_common_stub:
    pusha               ; Pushes eax, ecx, edx, ebx, esp, ebp, esi, edi onto the stack

    mov ax, ds          ; Stores the current data segment selector in ax
    push eax

    mov ax, 0x10        ; Loads the kernel data segment selector (0x10) into ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp            ; Passes the pointer to the registers structure (esp) as an argument to isr_handler (C code)
    call isr_handler
    add esp, 4          ; Cleans the stack after the call to isr_handler (removes the argument)

    pop eax             ; Restores the original data segment selector from the stack into ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                ; Restores the registers pushed by pusha (eax, ecx, edx, ebx, esp, ebp, esi, edi)
    add esp, 8          ; Cleans the stack after the call to isr_handler (removes int_no and err_code pushed by the stub, 2 * 4 bytes)
    ; sti                 ; Enables interrupts
    iret                ; Returns from the interrupt, restoring eip, cs, eflags, useresp, and ss from the stack

; Common handler stub for all IRQs. This code is executed after the CPU pushes the registers and before calling the C handler.
extern irq_handler

irq_common_stub:
    pusha

    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler
    add esp, 4

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8
    ; sti
    iret