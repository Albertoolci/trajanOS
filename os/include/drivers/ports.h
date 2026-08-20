// ports.h
#ifndef PORTS_H
#define PORTS_H

// Reads a byte (8 bits) from the specified I/O port
unsigned char inb(unsigned short port);

// Writes a byte (8 bits) to the specified I/O port
void outb(unsigned short port, unsigned char data);

// Reads a word (16 bits) from the specified I/O port
unsigned short inw(unsigned short port);

// Writes a word (16 bits) to the specified I/O port
void outw(unsigned short port, unsigned short data);

#endif