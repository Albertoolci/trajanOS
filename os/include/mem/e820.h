// include/mem/e820.h
#ifndef E820_H
#define E820_H

#define E820_LOCATION       0x8000
#define E820_TYPE_USABLE    1
#define E820_TYPE_RESERVED  2

// Structure representing a single entry returned by BIOS INT 0x15 E820
typedef struct {
    unsigned long long base_addr;
    unsigned long long length;
    unsigned int type;
} __attribute__((packed)) e820_entry_t;

// Memory map info container populated in bootloader
typedef struct {
    unsigned int count;
    e820_entry_t entries[32]; // Max entries stored at 0x8004
} __attribute__((packed)) e820_map_t;

#endif