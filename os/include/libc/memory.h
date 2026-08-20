// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void memcpy(char* source, char* dest, int no_bytes);
void memset(char* dest, char val, int len);

#endif