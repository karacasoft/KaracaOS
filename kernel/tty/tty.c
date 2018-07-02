#include <tty/tty.h>

extern tty_handle_t vga_ttyhandle;

extern void arch_tty_init(void);

void tty_setcharwithcolor(uint16_t * const address, char ch, uint8_t color);
void tty_scroll(tty_handle_t *handle);

void tty_init(void)
{
    arch_tty_init();
}

tty_handle_t *tty_getdefaulthandle(void)
{
    return &vga_ttyhandle;
}

void tty_setcolors(tty_handle_t *handle, uint8_t foreground, uint8_t background)
{
    handle->color = (background << 4) | (foreground & 0x0F);
}

void tty_setcharwithcolor(uint16_t *address, char ch, uint8_t color)
{
    uint16_t character = (uint16_t) ((ch) | (color << 8));
    *address = character;
}

void tty_scroll(tty_handle_t *handle)
{
    int x, y;
    uint16_t *address = handle->getbufferptr();
    for(y = 1; y < handle->height; y++)
    {
        for(x = 0; x < handle->width; x++)
        {
            address[(y - 1) * handle->width + x] = address[y * handle->width + x];
        }
    }

    for(x = 0; x < handle->width; x++)
    {
        address[(handle->height - 1) * handle->width + x] = 0x0000;
    }
}

void tty_putch(tty_handle_t *handle, char ch)
{
    if(ch == '\n')
    {
        handle->cursorx = handle->width - 1;
    }
    else if(ch == '\r')
    {
        handle->cursorx = 0;
    }
    else if(ch == '\b')
    {
        size_t index = handle->cursory * handle->width + handle->cursorx;
        handle->getbufferptr()[index] = 0x0000;
        while((handle->getbufferptr()[index--] & 0xFF) == '\0')
        {
            if(--handle->cursorx < 0)
            {
                if(--handle->cursory < 0)
                {
                    handle->cursory = 0;
                }
            }
        }

    }
    else if(ch == '\t')
    {
        handle->cursorx = handle->cursorx + (8 - (handle->cursorx % 8)) - 1;
    }
    else
    {
        tty_setcharwithcolor(handle->getbufferptr() +
            (handle->cursory * handle->width + handle->cursorx),
            ch, handle->color);
    }
    
    if(++handle->cursorx >= handle->width)
    {
        handle->cursorx = 0;
        if(++handle->cursory >= handle->height)
        {
            tty_scroll(handle);
            handle->cursory = handle->height - 1;
        }
    }
}

size_t tty_puts(tty_handle_t *handle, const char *str, size_t length)
{
    size_t count = 0;
    while(*str != 0 && length-- != 0)
    {
        tty_putch(handle, *str++);
        count++;
    }
    return count;
}
