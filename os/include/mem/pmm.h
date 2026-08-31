#ifndef PMM_H
#define PMM_H

#include "e820.h"

#define PMM_BLOCK_SIZE        4096 // 4 KB page size
#define PMM_BLOCKS_PER_BYTE   8

void pmm_init();
void *pmm_alloc_block();
void pmm_free_block(void *p);

// Internal bitwise manipulation helper functions
void pmm_set_bit(unsigned int bit);
void pmm_clear_bit(unsigned int bit);
int  pmm_test_bit(unsigned int bit);
int  pmm_find_first_free_bit();

// Getters for memory statistics
unsigned int pmm_get_total_blocks();
unsigned int pmm_get_used_blocks();
unsigned int pmm_get_free_blocks();

#endif