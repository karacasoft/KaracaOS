#include <tty/tty.h>
#include <interrupts/interrupts.h>
#include <kernel/sysdefs.h>

static const char *welcomemessage = SYSTEM_NAME " version 0.0.1\n";

int divisionByZeroExceptionHandler(uint32_t intNo, uint32_t errCode, void *frame) {
  tty_handle_t *tty = tty_getdefaulthandle();

  tty_puts(tty, "A division error has occured\n", 30);
  return 0;
}

void require(int nr) {
  (void *)nr;
}

void kmain(void)
{
    tty_init();

    tty_handle_t *tty = tty_getdefaulthandle();

    tty_setcolors(tty, VGA_COLOR_GRAY, VGA_COLOR_BLACK);
    tty_puts(tty, welcomemessage, 30);

    interrupts_init();
    interrupts_sethandler(0, divisionByZeroExceptionHandler);
    interrupts_enableinterrupts();

    int a = 5;

    tty_puts(tty, "Hmm\n", 5);

/*    int i;
    for(i = 0; i < 30; i++)
    {
        tty_puts(tty, "Testing the \\n\n", sizeof("Testing the \\n\n"));
    }*/


    while(TRUE);
}
