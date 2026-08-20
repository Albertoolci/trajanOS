// memory.c
#include "../../include/libc/memory.h"

/**
 * @brief Copies a block of memory from the source to the destination.
 * @details This function copies `number_bytes` bytes from the memory area pointed to by `source` to the memory area pointed to by `dest`. The memory areas must not overlap. If they do, use `memmove` instead.
 * @param source Pointer to the source memory area.
 * @param dest Pointer to the destination memory area.
 * @param number_bytes The number of bytes to copy.
 * @return None
 */
void memcpy(char *source, char *dest, int number_bytes) {
    for (int i = 0; i < number_bytes; i++) {
        dest[i] = source[i];
    }
}

/** 
 * @brief Fills a block of memory with a specified value.
 * @details This function sets the first `number_bytes` bytes of the memory area pointed to by `dest` to the specified value `val`.
 * @param dest Pointer to the memory area
 * @param val The value to set
 * @param number_bytes The number of bytes to set
 * @return None
 */
void memset(char *dest, char val, int number_bytes) {
    for (int i = 0; i < number_bytes; i++) {
        dest[i] = val;
    }
}