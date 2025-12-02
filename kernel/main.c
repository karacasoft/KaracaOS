#include "arch/paging/paging.h"
#include <mm/buddy_allocator.h>
#include <interrupts/interrupts.h>
#include <kernel/sysdefs.h>
#include <libc/stdio.h>
#include <mm/mm.h>
#include <multiboot.h>
#include <tty/tty.h>

#include <dev/keyboard/keyboard.h>
#include <fs/fscontext.h>

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

  kaos_preboot_printf("Page fault exception caused while trying to access 0x%x\n",
              addr);
  kaos_preboot_printf("The error code is: 0x%x\n", errCode);

  while (1)
    ;

  return INT_RETURN_NO_ERROR;
}

extern SYS_RET arch_paging_virt_to_phys(uint32_t *phys_addr, uint32_t virt_addr);

void kmain(multiboot_info_t *info, unsigned int magic) {
  tty_init();

  tty_handle_t *tty = tty_getdefaulthandle();

  tty_setcolors(tty, VGA_COLOR_GRAY, VGA_COLOR_BLACK);
  tty_puts(tty, welcomemessage, 30);

  mm_init(info);

  // Move kernel stack to its designated location
  void *stackTop;
  void *stackBottom;
  mm_alloc(&stackTop, 0xCFF00000, 0x10000, 0);
  stackBottom = stackTop + 0xFFFC;
  asm volatile ("mov %0, %%esp\npush $0\npush $0": : "r"(stackBottom));

  interrupts_init();
  interrupts_sethandler(0, divisionByZeroExceptionHandler);
  interrupts_sethandler(14, pageFaultHandler);

  buddy_allocator__init();
  __kaos_init_stdlib();
  keyboard_initialize();
  interrupts_enableinterrupts();

  fscontext__init();

  kaos_printf("Init complete\n");
  int result = my_main();

  /*    int i;
      for(i = 0; i < 30; i++)
      {
          tty_puts(tty, "Testing the \\n\n", sizeof("Testing the \\n\n"));
      }*/

  while (TRUE)
    ;
}
