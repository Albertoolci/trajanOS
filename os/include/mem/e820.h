// include/mem/e820.h
#ifndef E820_H
#define E820_H

#define E820_LOCATION       0x8000
#define E820_TYPE_USABLE    1
#define E820_TYPE_RESERVED  2

#include <stdint.h>

// Structure representing a single entry returned by BIOS INT 0x15 E820
typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
} __attribute__((packed)) e820_entry_t;

// Memory map info container populated in bootloader
typedef struct {
    uint32_t count;
    e820_entry_t entries[32]; // Max entries stored at 0x8004
} __attribute__((packed)) e820_map_t;

#endif