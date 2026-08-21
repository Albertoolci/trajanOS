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
        unsigned int *d4 = (unsigned int *)d;
        const unsigned int *s4 = (const unsigned int *)s;
        for (int i = 0; i < number_bytes / 4; i++) {
            d4[i] = s4[i];
        }
    }

    // If source, destination and number of bytes are alligned with a WORD (2 bytes), copy 2 bytes at a time
    else if ((d % 2 == 0) && (s % 2 == 0) && (number_bytes % 2 == 0)) {
        unsigned short *d2 = (unsigned short *)d;
        const unsigned short *s2 = (const unsigned short *)s;
        for (int i = 0; i < number_bytes / 2; i++) {
            d2[i] = s2[i];
        }
    } else {
        unsigned char *d1 = (unsigned char *)d;
        const unsigned char *s1 = (const unsigned char *)s;
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
        unsigned int *d4 = (unsigned int *)d;
        unsigned int dword_val = (unsigned char)val | ((unsigned char)val << 8) | ((unsigned char)val << 16) | ((unsigned char)val << 24);
        for (int i = 0; i < number_bytes / 4; i++) {
            d4[i] = dword_val;
        }
    }

    // If destination and number of bytes are alligned with a WORD (2 bytes), set 2 bytes at a time
    else if ((d % 2 == 0) && (number_bytes % 2 == 0)) {
        unsigned short *d2 = (unsigned short *)d;
        unsigned short word_val = (unsigned char)val | ((unsigned char) val << 8);
        for (int i = 0; i < number_bytes / 2; i++) {
            d2[i] = word_val;
        }
    }

    // If destination and number of bytes are not alligned, set 1 byte at a time
    else {
        unsigned char *d1 = (unsigned char *)d;
        unsigned char byte_val = (unsigned char)val;
        for (int i = 0; i < number_bytes; i++) {
            d1[i] = byte_val;
        }
    }
    return dest;
}