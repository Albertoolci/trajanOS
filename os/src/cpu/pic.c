// pic.c
#include "../../include/cpu/pic.h"
#include "../../include/drivers/ports.h"

/**
 * @brief Remaps the PIC (Programmable Interrupt Controller) to avoid conflicts with CPU exceptions.
 * @details This function reprograms the PIC to use interrupt vectors starting from 32 (0x20) for IRQ0-IRQ7 and 40 (0x28) for IRQ8-IRQ15. It also saves and restores the current interrupt masks.
 * @return None
 */
void pic_remap() {
    unsigned char mask1 = inb(PIC1_DATA); // Save masks
    unsigned char mask2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, 0x11); // Start initialization sequence in cascade mode
    outb(PIC2_COMMAND, 0x11);

    outb(PIC1_DATA, 0x20); // Remap PIC1 interrupts to 32-39
    outb(PIC2_DATA, 0x28); // Remap PIC2 interrupts to 40-47

    outb(PIC1_DATA, 0x04); // Tell PIC1 that there is a slave PIC at IRQ2
    outb(PIC2_DATA, 0x02); // Tell PIC2 its cascade identity

    outb(PIC1_DATA, 0x01); // Set PIC1 to 8086/88 mode
    outb(PIC2_DATA, 0x01); // Set PIC2 to 8086/88 mode

    outb(PIC1_DATA, mask1); // Restore saved masks
    outb(PIC2_DATA, mask2);
}

/**
 * @brief Sends an End of Interrupt (EOI) signal to the PIC.
 * @details This function sends an EOI signal to the appropriate PIC based on the IRQ number.
 * @param irq The interrupt request number.
 * @return None
 */
void pic_send_eoi(unsigned char irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI); // Send EOI to slave PIC
    }

    outb(PIC1_COMMAND, PIC_EOI); // Always send EOI to master PIC
}