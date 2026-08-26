// keyboard.c
#include "../../include/drivers/keyboard.h"
#include "../../include/drivers/ports.h"
#include "../../include/drivers/screen.h"
#include "../../include/cpu/isr.h"

static const char scancode_ascii_us[] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',    0, ' '
};

static const char scancode_ascii_es[] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', '!', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '`',  '+', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0 /*ñ*/, '{',  '|',
    0,  '<', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '-',    0,
  '*',    0, ' '
};

/**
 * @brief Callback function for keyboard interrupts.
 * @details This function is called every time a keyboard interrupt occurs. It reads the scancode from the keyboard data port, checks if it's a key press (ignoring key releases), and translates the scancode to an ASCII character. If a valid ASCII character is obtained, it is printed to the screen using the kprintf function.
 * @param regs A structure containing the CPU state at the time of the interrupt.
 * @return None
 */
static void keyboard_callback(registers_t *regs) {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);

    if (scancode & 0x80) {
        return; // Ignore key release events
    }

    if (scancode < sizeof(scancode_ascii_es)) {
        unsigned char ascii_char = scancode_ascii_es[scancode];
        if (ascii_char) {
            kprintf("%c", ascii_char);
        }
    }
}

/**
 * @brief Initializes the keyboard driver and registers the keyboard interrupt handler.
 * @details This function registers the keyboard_callback function as the interrupt handler for keyboard interrupts.
 * @return None
 */
void init_keyboard() {
    register_interrupt_handler(33, keyboard_callback);
}