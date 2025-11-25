#include <kernel/sysdefs.h>
#include <arch/iobase.h>
#include <interrupts/interrupts.h>

#include <dev/keyboard/keyboard.h>

#include <arch/keyboard/scancode_resolve.h>

int_return_t arch_keyboard_interrupt_handler(uint32_t intNo, uint32_t errCode, void *fullStackFrame)
{
  if(intNo == 33) {
    keyboard_interrupt_handler(scancode_resolve_getKeyCode(1));
    return INT_RETURN_NO_ERROR;
  }
  return INT_RETURN_WRONG_HANDLER;
}

SYS_RET arch_keyboard_initialize()
{
  interrupts_sethandler(33, arch_keyboard_interrupt_handler);
  //interrupts_enablegate(33);
  return SYS_RET_NO_ERROR;
}
