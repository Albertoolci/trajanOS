// string.h
#ifndef STRING_H
#define STRING_H

int strlen(const char* s);
void reverse(char* s);
int itoa(int n, char* str, int max_len);
int hex_to_ascii(unsigned int n, char* str, int max_len);
int strcmp(const char* s1, const char* s2);

#endif