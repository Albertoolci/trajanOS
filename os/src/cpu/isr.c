// isr.c
#include "../../include/cpu/isr.h"
#include "../../include/drivers/screen.h"
#include "../../include/cpu/idt.h"
#include "../../include/cpu/pic.h"

// Array of function pointers to hold custom interrupt handlers for each interrupt number (0-255).
isr_t interrupt_handlers[256];

// Exception messages corresponding to the 32 CPU exceptions.
static const char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Security Exception",
    "Reserved"
};

/**
 * @brief Relates all stubs to its corresponding interrupt index in the IDT.
 * @details This function sets up the Interrupt Descriptor Table (IDT) entries for the 32 CPU interrupts (0-31) and the 16 IRQs (32-47). It uses the set_idt_gate function to configure each entry in the IDT.
 * @return None
 */
void isr_install() {
    clean_idt();
    init_idt();

    set_idt_gate(0, (unsigned int)isr0);
    set_idt_gate(1, (unsigned int)isr1);
    set_idt_gate(2, (unsigned int)isr2);
    set_idt_gate(3, (unsigned int)isr3);
    set_idt_gate(4, (unsigned int)isr4);
    set_idt_gate(5, (unsigned int)isr5);
    set_idt_gate(6, (unsigned int)isr6);
    set_idt_gate(7, (unsigned int)isr7);
    set_idt_gate(8, (unsigned int)isr8);
    set_idt_gate(9, (unsigned int)isr9);
    set_idt_gate(10, (unsigned int)isr10);
    set_idt_gate(11, (unsigned int)isr11);
    set_idt_gate(12, (unsigned int)isr12);
    set_idt_gate(13, (unsigned int)isr13);
    set_idt_gate(14, (unsigned int)isr14);
    set_idt_gate(15, (unsigned int)isr15);
    set_idt_gate(16, (unsigned int)isr16);
    set_idt_gate(17, (unsigned int)isr17);
    set_idt_gate(18, (unsigned int)isr18);
    set_idt_gate(19, (unsigned int)isr19);
    set_idt_gate(20, (unsigned int)isr20);
    set_idt_gate(21, (unsigned int)isr21);
    set_idt_gate(22, (unsigned int)isr22);
    set_idt_gate(23, (unsigned int)isr23);
    set_idt_gate(24, (unsigned int)isr24);
    set_idt_gate(25, (unsigned int)isr25);
    set_idt_gate(26, (unsigned int)isr26);
    set_idt_gate(27, (unsigned int)isr27);
    set_idt_gate(28, (unsigned int)isr28);
    set_idt_gate(29, (unsigned int)isr29);
    set_idt_gate(30, (unsigned int)isr30);
    set_idt_gate(31, (unsigned int)isr31);

    set_idt_gate(32, (unsigned int)irq0); 
    set_idt_gate(33, (unsigned int)irq1);
    set_idt_gate(34, (unsigned int)irq2); 
    set_idt_gate(35, (unsigned int)irq3);
    set_idt_gate(36, (unsigned int)irq4);  
    set_idt_gate(37, (unsigned int)irq5);
    set_idt_gate(38, (unsigned int)irq6);  
    set_idt_gate(39, (unsigned int)irq7);
    set_idt_gate(40, (unsigned int)irq8);  
    set_idt_gate(41, (unsigned int)irq9);
    set_idt_gate(42, (unsigned int)irq10); 
    set_idt_gate(43, (unsigned int)irq11);
    set_idt_gate(44, (unsigned int)irq12); 
    set_idt_gate(45, (unsigned int)irq13);
    set_idt_gate(46, (unsigned int)irq14); 
    set_idt_gate(47, (unsigned int)irq15);

    for (int i = 0; i < 256; i++) {
        interrupt_handlers[i] = 0;
    }

    load_idt();
}

/**
 * @brief Handles CPU exceptions and interrupts.
 * @details This function is called when an interrupt or exception occurs. It receives a structure containing the CPU state at the time of the interrupt. If the interrupt number is less than 32, it prints the corresponding exception message and error code. For other interrupts, it enters an infinite loop to halt the CPU, as they are not handled in this implementation.
 * @param regs A structure containing the CPU state at the time of the interrupt.
 * @return None
 */
void isr_handler(registers_t *regs) {
    if (regs->int_no < 32) {
        // For the moment, just print the exception message
        kprintf("An exception occurred: %s\n", exception_messages[regs->int_no]);
        kprintf("Exception Number: %d\n", regs->int_no);
        kprintf("Error Code: %d\n", regs->err_code);

        while(1) { // Halt the CPU after printing the exception message
            __asm__ volatile("cli; hlt");
        }

    } else {
        // Halts the CPU if an unhandled interrupt occurs
        while(1);
    }
}

/**
 * @brief Registers a custom interrupt handler for a specific interrupt number.
 * @details This function allows the registration of a custom interrupt handler for a specific interrupt number. The handler is stored in the `interrupt_handlers` array, which is indexed by the interrupt number.
 * @param n The interrupt number for which to register the handler.
 * @param handler The interrupt handler function to register.
 * @return None
 */
void register_interrupt_handler(unsigned char n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

/**
 * @brief Handles IRQs (Interrupt Requests) from hardware devices.
 * @details This function is called when an IRQ occurs. It checks if a custom handler is registered for the IRQ and calls it if present. After handling the IRQ, it sends an End of Interrupt (EOI) signal to the PIC to notify that the interrupt has been processed.
 * @param regs A structure containing the CPU state at the time of the interrupt.
 * @return None
 */
void irq_handler(registers_t *regs) {
    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }

    pic_send_eoi(regs->int_no - 32);
}

/**
 * @brief Initializes the interrupt system by installing ISRs and remapping the PIC.
 * @details This function sets up the interrupt system by installing the Interrupt Service Routines (ISRs) and remapping the Programmable Interrupt Controller (PIC) to avoid conflicts with CPU exceptions. It ensures that the system is ready to handle interrupts from both software and hardware sources.
 * @return None
 */
void interrupts_init() {
    isr_install();
    pic_remap();

    // __asm__ volatile("sti");
}