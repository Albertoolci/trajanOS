// screen.c
#include "../../include/drivers/screen.h"
#include "../../include/drivers/ports.h"
#include "../../include/libc/memory.h"
#include "../../include/libc/string.h"
#include <stdarg.h>

static int get_cursor_offset();
static void set_cursor_offset(int offset);
static int print_char(char c, int col, int row, char attr);
static int handle_scrolling(int cursor_offset);

/**
 * @brief Clears the screen by filling it with spaces and resetting the cursor position.
 * @details This function clears the entire screen by writing spaces to all character cells and sets the cursor position to the top-left corner (0, 0). It uses the VGA text mode memory address to perform the operation.
 * @param None
 * @return None
 */
void clear_screen() {
    char *vga = (char*) VIDEO_ADDRESS;
    int screen_size = MAX_ROWS * MAX_COLS * 2; // Each character cell consists of 2 bytes (character + attribute)
    for (int i = 0; i < screen_size; i = i + 2) {
        vga[i] = ' ';
        vga[i + 1] = WHITE_ON_BLACK;
    }
    set_cursor_offset(0);
}

/**
 * @brief Prints a message at the specified column and row on the screen.
 * @details Prints a null-terminated string `message` at the specified column (`col`) and row (`row`) on the screen. If the column and row are negative, it prints at the current cursor position.
 * @param message The null-terminated string to be printed.
 * @param col The column number (0-based) where the message should start.
 * @param row The row number (0-based) where the message should start.
 * @return None
 */
void kprint_at(const char* message, int col, int row) {
    int offset;
    if (col >= 0 && row >= 0) {
        offset = (row * MAX_COLS + col) * 2;
        set_cursor_offset(offset);
    }

    for (int i = 0; message[i] != '\0'; i++) {
        print_char(message[i], -1, -1, WHITE_ON_BLACK);
    }
}

/**
 * @brief Prints a message at the current cursor position on the screen.
 * @details Prints a null-terminated string `message` at the current cursor position on the screen.
 * @param message The null-terminated string to be printed.
 * @return None
 */
void kprint(const char* message) {
    kprint_at(message, -1, -1);
}

/**
 * @brief Prints a character at the specified column and row with the given color attribute and updates the cursor position.
 * @details This function prints a single character `c` at the specified column (`col`) and row (`row`) on the screen with the given color attribute `color`. If the column and row are negative, it prints at the current cursor position.
 * @param c The character to be printed.
 * @param col The column number (0-based) where the character should be printed.
 * @param row The row number (0-based) where the character should be printed.
 * @param color The color attribute for the character (e.g., WHITE_ON_BLACK).
 * @return The new cursor offset after printing the character.
 */
static int print_char(char c, int col, int row, char color) {
    char* vga = (char*) VIDEO_ADDRESS;
    int offset = col >= 0 && row >= 0 ? (row * MAX_COLS + col) * 2 : get_cursor_offset();
    
    if (c == '\n') {
        int which_row = offset / (2 * MAX_COLS);
        offset = (which_row + 1) * MAX_COLS * 2;
    } else {
        vga[offset] = c;
        vga[offset + 1] = color;
        offset += 2;
    }

    offset = handle_scrolling(offset);
    set_cursor_offset(offset);
    return offset;
}

/**
 * @brief Handles scrolling of the screen when the cursor goes beyond the last row.
 * @details This function checks if the cursor offset exceeds the maximum screen size. If it does, it scrolls the screen up by one row, effectively removing the top row and making space for new content at the bottom. It returns the new cursor offset after scrolling.
 * @param cursor_offset The current cursor offset in bytes.
 * @return The new cursor offset after handling scrolling.
 */
static int handle_scrolling(int cursor_offset) {
    if (cursor_offset < MAX_ROWS * MAX_COLS * 2) {
        return cursor_offset;
    }

    // Copies lines 1..24 to lines 0..23
    char* vga = (char*) VIDEO_ADDRESS;
    for (int i = 1; i < MAX_ROWS; i++) {
        memcpy(vga + (i - 1) * MAX_COLS * 2, vga + i * MAX_COLS * 2, MAX_COLS * 2);
    }

    // Clears the last line
    char* last_line = vga + (MAX_ROWS - 1) * MAX_COLS * 2;
    for (int i = 0; i < MAX_COLS * 2; i += 2) {
        last_line[i] = ' ';
        last_line[i + 1] = WHITE_ON_BLACK;
    }

    return (int)(last_line - vga);
}

/**
 * @brief Gets the current cursor offset.
 * @details This function asks the VGA controller for the current cursor position and returns the offset in bytes.
 * @return The current cursor offset in bytes.
 */
static int get_cursor_offset() {
    // Asks the VGA controller for the current cursor position (high byte)
    outb(REG_SCREEN_CTRL, 14);
    // Stores the high byte of the cursor offset
    int reg_high = inb(REG_SCREEN_DATA) << 8;
    // Asks the VGA controller for the current cursor position (low byte)
    outb(REG_SCREEN_CTRL, 15);
    // Stores the low byte of the cursor offset
    int reg_low = inb(REG_SCREEN_DATA);
    // Combines the high and low bytes to get the full cursor offset in bytes (*2 because has to contain the color attribute)
    int offset = (reg_high | reg_low) * 2;

    return offset;
}
/**
 * @brief Sets the cursor offset.
 * @details This function sets the cursor position in the VGA text mode by writing to the VGA controller.
 * @param offset The new cursor offset in bytes.
 */
static void set_cursor_offset(int offset) {
    offset = offset / 2;
    uint8_t reg_high = (uint8_t)(offset >>8);
    uint8_t reg_low = (uint8_t)(offset & 0xFF);
    outb(REG_SCREEN_CTRL, 14);
    outb(REG_SCREEN_DATA, reg_high);
    outb(REG_SCREEN_CTRL, 15);
    outb(REG_SCREEN_DATA, reg_low);
}

void kprintf(const char* format, ...) {
    if (format == 0) return;

    va_list args;
    va_start(args, format);

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%') {
            i++;

            // If the format string ends with a '%' character, we print it literally and break the loop
            if (format[i] == '\0') {
                kprint("%");
                break;
            }

            switch (format[i]) {
                case 's': {
                    char* s = va_arg(args, char*);
                    if (s == 0) {
                        kprint("(null)");
                    } else {
                        kprint(s);
                    }
                    break;
                }
                case 'd': {
                    int d = va_arg(args, int);
                    char buffer[16]; // 16 bytes should be enough for a 32-bit integer and the null terminator
                    if (itoa(d, buffer, sizeof(buffer)) == 0) {
                        kprint(buffer);
                    } else {
                        kprint("[ERR]");
                    }
                    break;
                }
                case 'x': {
                    unsigned int x = va_arg(args, unsigned int);
                    char buffer[12]; // 12 bytes should be enough for "0x" + 8 hex digits + '\0'
                    if (hex_to_ascii(x, buffer, sizeof(buffer)) == 0) {
                        kprint(buffer);
                    } else {
                        kprint("[ERR]");
                    }
                    break;
                }
                case 'c': {
                    // We use va_arg with int because char is promoted to int when passed through '...'
                    char c = (char) va_arg(args, int);
                    char str[2] = {c, '\0'};
                    kprint(str);
                    break;
                }
                case '%': {
                    // Allows printing a literal '%' character by using '%%' in the format string
                    kprint("%");
                    break;
                }
                default: {
                    // If the format specifier is unrecognized, we print it literally with a '%' prefix
                    char str[3] = {'%', format[i], '\0'};
                    kprint(str);
                    break;
                }
            }
        } else {
            char str[2] = {format[i], '\0'};
            kprint(str);
        }
    }

    va_end(args);
}


/**
 * @brief Handles the backspace operation on the screen.
 * @details This function moves the cursor back by one character position, replaces the character at that position with a space, and updates the cursor position accordingly. It ensures that the cursor does not move beyond the beginning of the screen.
 * @param None
 * @return None
 */
void backspace() {
    int offset = get_cursor_offset();
    if (offset == 0) {
        return; // Cursor is at the beginning of the screen, nothing to backspace
    }

    offset -= 2; // Move back one character (2 bytes for character and attribute)
    set_cursor_offset(offset);

    char* vga = (char*) VIDEO_ADDRESS;
    vga[offset] = ' '; // Replace the character with a space
    vga[offset + 1] = WHITE_ON_BLACK; // Reset the color attribute
}