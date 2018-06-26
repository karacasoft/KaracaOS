#include "vga_text.h"

#include <tty/tty.h>
#include <kernel/sysdefs.h>

BOOLEAN vga_tty_initialized = FALSE;
tty_handle_t vga_ttyhandle;

void arch_tty_init(void);

void arch_tty_init(void)
{
    if(!vga_tty_initialized)
    {
        vga_ttyhandle.width = 80;
        vga_ttyhandle.height = 25;
        vga_ttyhandle.cursorx = 0;
        vga_ttyhandle.cursory = 0;
        vga_ttyhandle.color = 0;
        vga_ttyhandle.bufferptr = (uint16_t *)(VGA_MEM_ADDR);



        vga_tty_initialized = TRUE;
    }
}
