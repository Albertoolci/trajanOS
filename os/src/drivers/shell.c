// shell.c
#include "../../include/drivers/shell.h"
#include "../../include/drivers/screen.h"
#include "../../include/libc/string.h"
#include "../../include/cpu/timer.h"

/**
 * @brief Prints the shell prompt to the screen.
 * @details This function prints the shell prompt "trajanOS > " to the screen, indicating that the shell is ready to accept user input.
 * @return None
 */
static void print_prompt() {
    kprintf("trajanOS > ");
}

/**
 * @brief Initializes the shell by printing the welcome message and the prompt.
 * @details This function prints the welcome message for the shell, including the version number and instructionsº for accessing help. It then calls the print_prompt function to display the shell prompt, indicating that the shell is ready to accept user input.
 * @return None
 */
void shell_init() {
    kprintf("\n--- TrajanOS Shell v0.1 ---\n");
    kprintf("Type 'help' in order to see the available commands.\n\n");
    print_prompt();
}

/**
 * @brief Parses the user input and executes the corresponding command.
 * @details This function takes the user input and compares it to a list of available commands. If a match is found, the corresponding action is taken.
 * @param input The user input to parse.
 * @return None
 */
void shell_parse(const char *input) {
    if (strcmp(input, "help") == 0) {
        kprintf("Available commands:\n");
        kprintf("  help     - Shows this help message\n");
        kprintf("  clear    - Clears the VGA screen\n");
        kprintf("  version  - Shows the kernel version\n");
        kprintf("  ticks    - Shows the number of ticks since the timer was initialized\n");
        kprintf("\n");
        print_prompt();
    } else if (strcmp(input, "clear") == 0) {
        clear_screen();
        print_prompt();
    } else if (strcmp(input, "version") == 0) {
        kprintf("trajanOS v0.1 (32-bit x86 Protected Mode Kernel)\n");
        kprintf("\n");
        print_prompt();
    } else if (strcmp(input, "ticks") == 0) {
        kprintf("Ticks: %d\n", get_ticks());
        kprintf("\n");
        print_prompt();
    } else if (strlen(input) > 0) {
        kprintf("Unknown command: '%s'\n", input);
        kprintf("\n");
        print_prompt();
    } else {
        print_prompt();
    }
}

