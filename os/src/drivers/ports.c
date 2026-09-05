// ports.c
#include "../../include/drivers/ports.h"

/**
 * @brief Reads a byte (8 bits) from the specified I/O port.
 * @details This function reads a byte from the I/O port specified by `port`. It uses inline assembly to perform the port I/O operation. The function returns the byte read from the port.
 * @param port The I/O port number to read from.
 * @return The byte read from the specified I/O port.
 */
uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__(
        "in %%dx, %%al" :
        "=a" (result) :
        "d" (port)
    );
    return result;
}

/**
 * @brief Writes a byte (8 bits) to the specified I/O port.
 * @details This function writes a byte to the I/O port specified by `port`. It uses inline assembly to perform the port I/O operation. The function does not return any value.
 * @param port The I/O port number to write to.
 * @param data The byte to write to the specified I/O port.
 */
void outb(uint16_t port, uint8_t data) {
    __asm__(
        "out %%al, %%dx" :
        :
        "a" (data), "d" (port)
    );
}

/**
 * @brief Reads a word (16 bits) from the specified I/O port.
 * @details This function reads a word from the I/O port specified by `port`. It uses inline assembly to perform the port I/O operation. The function returns the word read from the port.
 * @param port The I/O port number to read from.
 * @return The word read from the specified I/O port.
 */
uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__(
        "in %%dx, %%ax" :
        "=a" (result) :
        "d" (port)
    );
    return result;
}

/**
 * @brief Writes a word (16 bits) to the specified I/O port.
 * @details This function writes a word to the I/O port specified by `port`. It uses inline assembly to perform the port I/O operation. The function does not return any value.
 * @param port The I/O port number to write to.
 * @param data The word to write to the specified I/O port.
 */
void outw(uint16_t port, uint16_t data) {
    __asm__(
        "out %%ax, %%dx" :
        :
        "a" (data), "d" (port)
    );
}