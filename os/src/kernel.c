// kernel.c
#include "../include/drivers/ports.h"
#include "../include/drivers/screen.h"

void kernel_main() {
    clear_screen();

    kprintf("=== %s Booted Successfully ===\n", "TrajanOS");
    kprintf("Kernel Base: %x\n", 0x0500);
    kprintf("Video RAM:   %x\n", VIDEO_ADDRESS);
    kprintf("Status:      %s (Code: %d)\n\n", "READY", 200);

    // Scrolling test
    for (int i = 1; i <= 20; i++) {
        kprintf("Line test %d/20 - TrajanOS Console Output\n", i);
    }

    kprintf("\n\n=== Scrolling Test Successful, TrajanOS ===\n");

    while(1);
}