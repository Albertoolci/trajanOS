// string.h
#ifndef STRING_H
#define STRING_H

#include <stdint.h>

int strlen(const char* s);
void reverse(char* s);
int itoa(int n, char* str, int max_len);
int hex_to_ascii(uint32_t n, char* str, int max_len);
int strcmp(const char* s1, const char* s2);

#endif