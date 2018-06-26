#ifndef __TTY_H__
#define __TTY_H__ 1

#include <stdint.h>
#include <stddef.h>

#define VGA_COLOR_BLACK   0x0
#define VGA_COLOR_BLUE    0x1
#define VGA_COLOR_GREEN   0x2
#define VGA_COLOR_CYAN    0x3
#define VGA_COLOR_RED     0x4
#define VGA_COLOR_MAGENTA 0x5
#define VGA_COLOR_BROWN   0x6
#define VGA_COLOR_GRAY    0x7

#define VGA_COLOR_BRIGHT  0x8

typedef struct __tty_handle {
    uint16_t *bufferptr;
    int width;
    int height;

    int cursorx;
    int cursory;

    uint8_t color;
} tty_handle_t;

void tty_init(void);

tty_handle_t *tty_getdefaulthandle(void);

void tty_setcolors(tty_handle_t *handle, uint8_t foreground, uint8_t background);

void tty_putch(tty_handle_t *handle, char ch);
size_t tty_puts(tty_handle_t *handle, const char *str, size_t length);

#endif