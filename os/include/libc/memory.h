// memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

typedef uint32_t uintptr_t;

void* memcpy(void *dest, const void *source, int number_bytes);
void* memset(void *dest, int val, int number_bytes);

#endif