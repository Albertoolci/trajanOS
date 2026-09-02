// pmm.c
#include "../../include/mem/pmm.h"
#include "../../include/drivers/screen.h"

static unsigned int *pmm_bitmap = 0; // Bitmap to track used/free memory blocks
static unsigned int pmm_max_blocks = 0; // Total number of memory blocks
static unsigned int pmm_used_blocks = 0; // Number of used memory blocks

extern char _kernel_end[]; // End of the kernel, defined in the linker script

/**
 * @brief Sets the bit corresponding to the given block index in the bitmap, marking it as used.
 * @details This function calculates the index in the bitmap array and sets the appropriate bit to indicate that the block is now in use.
 * @param bit The index of the block to mark as used.
 * @return None
 */
void pmm_set_bit(unsigned int bit) {
    pmm_bitmap[bit / 32] |= (1 << (bit % 32));
}

/**
 * @brief Clears the bit corresponding to the given block index in the bitmap, marking it as free.
 * @details This function calculates the index in the bitmap array and clears the appropriate bit to indicate that the block is now free.
 * @param bit The index of the block to mark as free.
 * @return None
 */
void pmm_clear_bit(unsigned int bit) {
    pmm_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

/**
 * @brief Tests whether the bit corresponding to the given block index in the bitmap is set (used) or clear (free).
 * @details This function calculates the index in the bitmap array and checks the appropriate bit to determine if the block is currently in use or free.
 * @param bit The index of the block to test.
 * @return Returns 0 if the block is free, or a non-zero value if the block is used.
 */
int pmm_test_bit(unsigned int bit) {
    return pmm_bitmap[bit / 32] & (1 << (bit % 32));
}

/**
 * @brief Finds the index of the first free block in the bitmap.
 * @details This function iterates through the bitmap to find the first bit that is clear (indicating a free block). It returns the index of the first free block found, or -1 if no free blocks are available.
 * @return The index of the first free block, or -1 if no free blocks are available.
 */
int pmm_find_first_free_bit() {
    unsigned int bitmap_size = (pmm_max_blocks + 31) / 32;
    for (unsigned int i = 0; i < bitmap_size; i++) {
        if (pmm_bitmap[i] != 0xFFFFFFFF) {
            for (unsigned int j = 0; j < 32; j++) {
                unsigned int bit = 1 << j;
                if ((pmm_bitmap[i] & bit) == 0x00000000) {
                    int frame = i * 32 + j;
                    if (frame < pmm_max_blocks) {
                        return frame;
                    }
                }
            }
        }
    }

    return -1;
}

/**
 * @brief Allocates a single memory block from the physical memory manager.
 * @details This function finds the first free block in the bitmap and allocates it.
 * @return A pointer to the allocated memory block, or 0 if no free blocks are available.
 * @return None
 */
void *pmm_alloc_block() {
    if (pmm_used_blocks >= pmm_max_blocks) {
        return 0; // No free blocks available
    }

    int frame = pmm_find_first_free_bit();
    if (frame == -1) {
        return 0; // No free blocks found
    }

    pmm_set_bit(frame);
    pmm_used_blocks++;

    unsigned int addr = (unsigned int)frame * PMM_BLOCK_SIZE;
    return (void *)addr;
}

/**
 * @brief Frees a memory block, marking it as available in the bitmap.
 * @details This function calculates the block index from the given pointer and clears the corresponding bit in the bitmap to mark the block as free.
 * @param p A pointer to the memory block to be freed. The pointer must have been previously allocated by pmm_alloc_block().
 * @return None
 */
void pmm_free_block(void *p) {
    unsigned int addr = (unsigned int)p;
    unsigned int frame = addr / PMM_BLOCK_SIZE;

    if (frame >= pmm_max_blocks) {
        return; // Invalid block address
    }

    if (!pmm_test_bit(frame)) {
        return; // Block is already free
    }

    pmm_clear_bit(frame);
    pmm_used_blocks--;
}

/**
 * @brief Initializes the physical memory manager (PMM) using the E820 memory map.
 * @details This function reads the E820 memory map provided by the bootloader, determines the total number of memory blocks, initializes the bitmap, and marks usable memory blocks as free while protecting the kernel and bitmap areas.
 * @note This function should be called early in the kernel initialization process, before any memory allocations are performed.
 * @return None
 */
void pmm_init() {
    // Read the E820 memory map from the predefined location (0x8000)
    unsigned int *count_ptr = (unsigned int *)E820_LOCATION;
    unsigned int count = *count_ptr;
    e820_entry_t *entries = (e820_entry_t *)(E820_LOCATION + 4);

    unsigned long long max_memory = 0;

    // Define a local copy of the E820 entries to avoid potential issues with the original memory map being overwritten or modified during initialization
    e820_entry_t local_entries[32];
    unsigned int local_count = 0;

    if (count > 0 && count <= 32) {
        local_count = count;
        for (unsigned int i = 0; i < local_count; i++) {
            local_entries[i] = entries[i];
            if (local_entries[i].type == E820_TYPE_USABLE) {
                unsigned long long top = local_entries[i].base_addr + local_entries[i].length;
                if (top > max_memory) {
                    max_memory = top;
                }
            }
        }
    }

    if (max_memory == 0) {
        max_memory = 32 * 1024 * 1024;
    }

    pmm_max_blocks = (unsigned int)(max_memory / PMM_BLOCK_SIZE);
    pmm_used_blocks = pmm_max_blocks;

    // Initialize the bitmap at the end of the kernel's memory space
    pmm_bitmap = (unsigned int *)_kernel_end;
    unsigned int bitmap_size_bytes = (pmm_max_blocks + 7) / 8;
    
    unsigned int total_words = (pmm_max_blocks + 31) / 32;
    for (unsigned int b = 0; b < total_words; b++) {
        pmm_bitmap[b] = 0xFFFFFFFF;
    }

    // Mark usable memory blocks as free based on the E820 map
    if (local_count > 0) {
        for (unsigned int i = 0; i < local_count; i++) {
            if (local_entries[i].type == E820_TYPE_USABLE) {
                unsigned int start_block = (unsigned int)(local_entries[i].base_addr / PMM_BLOCK_SIZE);
                unsigned int block_count = (unsigned int)(local_entries[i].length / PMM_BLOCK_SIZE);

                for (unsigned int k = 0; k < block_count; k++) {
                    if ((start_block + k) < pmm_max_blocks) {
                        pmm_clear_bit(start_block + k);
                        pmm_used_blocks--;
                    }
                }
            }
        }
    } else {
        kprintf("Warning: No valid E820 map found. Freeing memory from 1 MB to end.\n");
        for (unsigned int k = 256; k < pmm_max_blocks; k++) {
            pmm_clear_bit(k);
            pmm_used_blocks--;
        }
    }

    // Protect the kernel and bitmap areas by marking them as used
    unsigned int kernel_and_bitmap_end = ((unsigned int)pmm_bitmap) + bitmap_size_bytes;
    unsigned int protected_blocks = (kernel_and_bitmap_end / PMM_BLOCK_SIZE) + 1;

    for (unsigned int b = 0; b < protected_blocks; b++) {
        if (b < pmm_max_blocks && !pmm_test_bit(b)) {
            pmm_set_bit(b);
            pmm_used_blocks++;
        }
    }
}

/**
 * @brief Returns the total number of memory blocks managed by the physical memory manager.
 * @details This function provides the total count of memory blocks that the PMM is managing, which is determined during the initialization process based on the system's available physical memory.
 * @return The total number of memory blocks.
 */
unsigned int pmm_get_total_blocks() {
    return pmm_max_blocks;
}

/**
 * @brief Returns the number of memory blocks currently in use.
 * @details This function provides the count of memory blocks that are currently allocated and in use, which is tracked by the PMM during allocation and deallocation operations.
 * @return The number of memory blocks currently in use.
 */
unsigned int pmm_get_used_blocks() {
    return pmm_used_blocks;
}

/**
 * @brief Returns the number of free memory blocks available for allocation.
 * @details This function calculates the number of free memory blocks by subtracting the number of used blocks from the total number of blocks managed by the PMM.
 * @return The number of free memory blocks available for allocation.
 */
unsigned int pmm_get_free_blocks() {
    return pmm_max_blocks - pmm_used_blocks;
}