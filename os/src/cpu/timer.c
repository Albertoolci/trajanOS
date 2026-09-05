// timer.c
#include "../../include/cpu/timer.h"
#include "../../include/drivers/screen.h"
#include "../../include/cpu/isr.h"
#include "../../include/drivers/ports.h"

static uint32_t tick = 0;

/**
 * @brief Callback function for the timer interrupt.
 * @details This function is called every time the timer interrupt occurs. It increments the tick count and prints the tick count every 100 ticks.
 * @param regs A structure containing the CPU state at the time of the interrupt.
 * @return None
 */
static void timer_callback(registers_t *regs) {
    tick++;
}

/**
 * @brief Returns the number of ticks since the timer was initialized.
 * @details This function returns the current tick count, which represents the number of timer interrupts that have occurred since the timer was initialized.
 * @return The number of ticks since the timer was initialized.
 */
uint32_t get_ticks() {
    return tick;
}

/**
 * @brief Initializes the system timer to generate interrupts at a specified frequency.
 * @details This function sets up the system timer to generate interrupts at the specified frequency. It registers the timer callback function to handle the timer interrupts and configures the timer hardware accordingly.
 * @param frequency The desired frequency of timer interrupts in Hertz (Hz).
 * @return None
 */
void init_timer(uint32_t frequency) {
    register_interrupt_handler(32, timer_callback);
    uint32_t divisor = 1193180 / frequency;
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
    outb(0x43, 0x36); // Command byte: 0x36 sets the timer to mode 3 (square wave generator)
    outb(0x40, low);
    outb(0x40, high);
}