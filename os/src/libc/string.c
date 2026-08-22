// string.c
#include "../../include/libc/string.h"

/**
 * @brief Calculates the length of a string.
 * @details This function computes the length of the string `s`, excluding the terminating null byte ('\0').
 * @param s Pointer to the null-terminated string.
 * @return The number of characters in the string, excluding the null terminator.
 */
int strlen(const char* s) {
    int length = 0;
    while (s[length] != '\0') {
        length++;
    }

    return length;
}

/**
 * @brief Reverses a string in place.
 * @details This function reverses the string `s` in place. The string must be null-terminated. The reversal is done by swapping characters from the start and end of the string.
 * @param s Pointer to the null-terminated string to be reversed.
 * @return None
 */
void reverse(char *s) {
    int start = 0;
    int end = strlen(s) - 1;

    while (start < end) {
        char aux = s[start];
        s[start] = s[end];
        s[end] = aux;
        start++;
        end--;
    }
}

/**
 * @brief Converts an integer to a string safely.
 * @param n The integer to convert.
 * @param s Pointer to the buffer.
 * @param max_len Maximum capacity of the buffer s.
 * @return 0 on success, -1 if inputs are invalid or output was truncated.
 */
int itoa(int n, char *s, int max_len) {
    // Bad buffer or insufficient space for at least one digit and null terminator
    if (s == 0 || max_len < 2) {
        return -1;
    }

    int i = 0;
    int is_negative = 0;
    int truncated = 0;
    unsigned int num;

    if (n < 0) {
        is_negative = 1;
        num = (unsigned int)(-n);
    } else {
        num = (unsigned int)n;
    }

    do {
        // If the buffer is too small to hold the next digit and the null terminator, we set truncated flag and break
        if (i >= max_len - (is_negative ? 2 : 1)) {
            truncated = 1;
            break;
        }
        s[i++] = (char)((num % 10) + '0');
        num /= 10;
    } while (num > 0);

    if (is_negative) {
        s[i++] = '-';
    }

    s[i] = '\0';
    reverse(s);

    // Retorna -1 si el número no cupo entero, o 0 si fue exitoso
    return truncated ? -1 : 0;
}

/**
 * @brief Converts an integer to a hexadecimal string safely.
 * @details Converts the integer `n` to a null-terminated hexadecimal string (with "0x" prefix)
 *          and stores it in the buffer pointed to by `s`.
 * @param n The integer to convert.
 * @param s Pointer to the buffer where the resulting string will be stored.
 * @param max_len The maximum capacity of the buffer `s` in bytes.
 * @return 0 on success, -1 if inputs are invalid or output was truncated.
 */
int hex_to_ascii(unsigned int n, char *s, int max_len) {
    // If the buffer is null or max_len is less than 2, return -1
    if (s == 0 || max_len < 2) {
        return -1;
    }

    int truncated = 0;
    int index = 0;

    // If the buffer is large enough to hold the "0x" prefix, write it
    if (max_len >= 4) {
        s[index++] = '0';
        s[index++] = 'x';
    } else {
        truncated = 1;
    }

    char zeros = 0;
    unsigned int tmp;

    // Parse each nibble (4 bits) of the integer, starting from the highest nibble (28th bit) down to the lowest (0th bit)
    for (int i = 28; i >= 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        
        // Ignore leading zeros until the first non-zero nibble is found
        if (tmp == 0 && zeros == 0) continue;
        
        zeros = 1;

        // Check if there is enough space in the buffer to write the next character and the null terminator
        if (index >= max_len - 1) {
            truncated = 1;
            break;
        }

        if (tmp > 0x9) {
            s[index++] = (char)((tmp - 0xA) + 'A');
        } else {
            s[index++] = (char)(tmp + '0');
        }
    }

    // If no non-zero nibbles were found, we need to write at least one '0' character
    if (!zeros) {
        if (index < max_len - 1) {
            s[index++] = '0';
        } else {
            truncated = 1;
        }
    }

    // Ensure the string is null-terminated
    s[index] = '\0';

    return truncated ? -1 : 0;
}