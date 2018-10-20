#include <tty/tty.h>
#include <kernel/sysdefs.h>

static const char *welcomemessage = SYSTEM_NAME " version 0.0.1\n";

void kmain(void)
{

    tty_init();

    tty_handle_t *tty = tty_getdefaulthandle();

    tty_setcolors(tty, VGA_COLOR_GRAY, VGA_COLOR_BLACK);
    tty_puts(tty, welcomemessage, 30);


/*    int i;
    for(i = 0; i < 30; i++)
    {
        tty_puts(tty, "Testing the \\n\n", sizeof("Testing the \\n\n"));
    }*/


    while(TRUE);
}
