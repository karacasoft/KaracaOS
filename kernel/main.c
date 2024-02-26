#include "mm/buddy_allocator.h"
#include <interrupts/interrupts.h>
#include <kernel/sysdefs.h>
#include <libc/stdio.h>
#include <mm/mm.h>
#include <multiboot.h>
#include <tty/tty.h>

#include <dev/keyboard/keyboard.h>

#include <kernel/test.h>

static const char *welcomemessage = SYSTEM_NAME " version 0.0.1\n";

int_return_t divisionByZeroExceptionHandler(uint32_t intNo, uint32_t errCode,
                                            void *frame) {
  tty_handle_t *tty = tty_getdefaulthandle();

  tty_puts(tty, "A division error has occured\n", 30);
  while (1)
    ;
  return INT_RETURN_NO_ERROR;
}

int_return_t pageFaultHandler(uint32_t intNo, uint32_t errCode, void *frame) {
  tty_handle_t *tty = tty_getdefaulthandle();
  uint32_t addr;

  asm("mov %%cr2, %0\n" : "=r"(addr));

  kaos_printf("Page fault exception caused while trying to access 0x%x\n",
              addr);
  kaos_printf("The error code is: 0x%x\n", errCode);

  while (1)
    ;

  return INT_RETURN_NO_ERROR;
}

void kmain(multiboot_info_t *info, unsigned int magic) {
  tty_init();

  tty_handle_t *tty = tty_getdefaulthandle();

  tty_setcolors(tty, VGA_COLOR_GRAY, VGA_COLOR_BLACK);
  tty_puts(tty, welcomemessage, 30);

  mm_init(info);

  interrupts_init();
  interrupts_sethandler(0, divisionByZeroExceptionHandler);
  interrupts_sethandler(14, pageFaultHandler);
  interrupts_enableinterrupts();

  keyboard_initialize();
  buddy_allocator__init();
  kaos_printf("Init complete??\n");

  int result = my_main();

  /*    int i;
      for(i = 0; i < 30; i++)
      {
          tty_puts(tty, "Testing the \\n\n", sizeof("Testing the \\n\n"));
      }*/

  while (TRUE)
    ;
}
