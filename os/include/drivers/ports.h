// ports.h
#ifndef PORTS_H
#define PORTS_H

#include <stdint.h>

// Reads a byte (8 bits) from the specified I/O port
uint8_t inb(uint16_t port);

// Writes a byte (8 bits) to the specified I/O port
void outb(uint16_t port, uint8_t data);

// Reads a word (16 bits) from the specified I/O port
uint16_t inw(uint16_t port);

// Writes a word (16 bits) to the specified I/O port
void outw(uint16_t port, uint16_t data);

#endif