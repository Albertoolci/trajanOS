// screen.h
#ifndef SCREEN_H
#define SCREEN_H

// VGA constants
#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80

// Color constants
#define WHITE_ON_BLACK 0x0F
#define RED_ON_BLACK   0x04
#define GREEN_ON_BLACK 0x02

// Screen control and data registers
#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5


void clear_screen();
void kprint_at(const char* message, int col, int row);
void kprint(const char* message);
void kprintf(const char* format, ...);

#endif