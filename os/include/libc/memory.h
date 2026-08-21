// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

typedef unsigned int uintptr_t;

void* memcpy(void *dest, const void *source, int number_bytes);
void* memset(void *dest, int val, int number_bytes);

#endif