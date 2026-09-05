// memory.c
#include "../../include/libc/memory.h"

/**
 * @brief Copies a block of memory from the source to the destination.
 * @details This function copies `number_bytes` bytes from the memory area pointed to by `source` 
 *          to the memory area pointed to by `dest`. The memory areas must not overlap.
 * @param dest Pointer to the destination memory area.
 * @param source Pointer to the source memory area.
 * @param number_bytes The number of bytes to copy.
 * @return Pointer to the destination memory area (`dest`).
 */
void* memcpy(void *dest, const void *source, int number_bytes) {
    uintptr_t d = (uintptr_t)dest;
    uintptr_t s = (uintptr_t)source;

    // If source, destination and number of bytes are alligned with a DWORD (4 bytes), copy 4 bytes at a time
    if ((d % 4 == 0) && (s % 4 == 0) && (number_bytes % 4 == 0)) {
        uint32_t *d4 = (uint32_t *)d;
        const uint32_t *s4 = (const uint32_t *)s;
        for (int i = 0; i < number_bytes / 4; i++) {
            d4[i] = s4[i];
        }
    }

    // If source, destination and number of bytes are alligned with a WORD (2 bytes), copy 2 bytes at a time
    else if ((d % 2 == 0) && (s % 2 == 0) && (number_bytes % 2 == 0)) {
        uint16_t *d2 = (uint16_t *)d;
        const uint16_t *s2 = (const uint16_t *)s;
        for (int i = 0; i < number_bytes / 2; i++) {
            d2[i] = s2[i];
        }
    } else {
        uint8_t *d1 = (uint8_t *)d;
        const uint8_t *s1 = (const uint8_t *)s;
        for (int i = 0; i < number_bytes; i++) {
            d1[i] = s1[i];
        }
    }
    return dest;
}

/** 
 * @brief Fills a block of memory with a specified value.
 * @details This function sets the first `number_bytes` bytes of the memory area pointed to by `dest` 
 *          to the specified value `val`.
 * @param dest Pointer to the memory area.
 * @param val The byte value to set (cast to unsigned char, only the least significant byte is used).
 * @param number_bytes The number of bytes to set.
 * @return Pointer to the memory area (`dest`).
 */
void* memset(void *dest, int val, int number_bytes) {
    uintptr_t d = (uintptr_t)dest;
    
    // If destination and number of bytes are alligned with a DWORD (4 bytes), set 4 bytes at a time
    if ((d % 4 == 0) && (number_bytes % 4 == 0)) {
        uint32_t *d4 = (uint32_t *)d;
        uint32_t dword_val = (uint8_t)val | ((uint8_t)val << 8) | ((uint8_t)val << 16) | ((uint8_t)val << 24);
        for (int i = 0; i < number_bytes / 4; i++) {
            d4[i] = dword_val;
        }
    }

    // If destination and number of bytes are alligned with a WORD (2 bytes), set 2 bytes at a time
    else if ((d % 2 == 0) && (number_bytes % 2 == 0)) {
        uint16_t *d2 = (uint16_t *)d;
        uint16_t word_val = (uint8_t)val | ((uint8_t) val << 8);
        for (int i = 0; i < number_bytes / 2; i++) {
            d2[i] = word_val;
        }
    }

    // If destination and number of bytes are not alligned, set 1 byte at a time
    else {
        uint8_t *d1 = (uint8_t *)d;
        uint8_t byte_val = (uint8_t)val;
        for (int i = 0; i < number_bytes; i++) {
            d1[i] = byte_val;
        }
    }
    return dest;
}