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
        end++;
    }
}

/**
 * @brief Converts an integer to a string.
 * @details This function converts the integer `n` to a null-terminated string and stores it in the buffer pointed to by `s`.
 * @param n The integer to convert.
 * @param s Pointer to the buffer where the resulting string will be stored. The buffer must be large enough to hold the resulting string, including the null terminator.
 * @return None
 */
void itoa(int n, char *s) {
    int i = 0;
    int is_negative = 0;

    if (n < 0) {
        is_negative = 1;
        n = -n;
    }

    do {
        s[i++] = (n % 10) + '0';
        n /= 10;
    } while (n > 0);

    if (is_negative) {
        s[i++] = '-';
    }

    s[i] = '\0';

    reverse(s);
}

/**
 * @brief Converts an integer to a hexadecimal string.
 * @details This function converts the integer `n` to a null-terminated hexadecimal string and stores it in the buffer pointed to by `s`.
 * @param n The integer to convert.
 * @param s Pointer to the buffer where the resulting string will be stored. The buffer must be large enough to hold the resulting string, including the null terminator.
 * @return None
 */
void hex_to_ascii(int n, char *s) {
    s[0] = '0';
    s[1] = 'x';

    char zeros = 0;
    int tmp;
    int index = 2;

    for (int i = 28; i >= 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp > 0x9) {
            s[index++] = (tmp - 0xA) + 'A';
        } else {
            s[index++] = tmp + '0';
        }
    }
    
    if (index == 2) {
        s[index++] = '0';
    }
    s[index] = '\0';
}