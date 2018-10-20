#include <interrupts/interrupts.h>
#include <tty/tty.h>

extern SYS_RET arch_interrupts_enableinterrupts();

extern SYS_RET arch_interrupts_enablegate(int gate);

extern SYS_RET arch_interrupts_setgenerichandler(generic_int_handler_t intHandler);
extern SYS_RET arch_interrupts_disablegate(int gate);

extern SYS_RET arch_interrupts_disableinterrupts();

extern SYS_RET arch_interrupts_init();

int_handler_t intHandlers[INTERRUPT_VECTOR_COUNT];

void interrupts_generic_handler(uint32_t int_number, uint32_t err_code, void *frame);

SYS_RET interrupts_init()
{
  interrupts_disableinterrupts();
  int i;
  for(i = 0; i < INTERRUPT_VECTOR_COUNT; i++)
  {
    intHandlers[i] = 0;
  }

  SYS_RET retVal = arch_interrupts_init();
  if(retVal) return retVal;

  arch_interrupts_setgenerichandler(interrupts_generic_handler);

  return SYS_RET_NO_ERROR;
}

SYS_RET interrupts_enableinterrupts()
{
  return arch_interrupts_enableinterrupts();
}

SYS_RET interrupts_enablegate(int gate)
{
  // TODO stub
  return arch_interrupts_enablegate(gate);
}

SYS_RET interrupts_sethandler(int gate, int_handler_t handler)
{
  intHandlers[gate] = handler;
  return SYS_RET_NO_ERROR;
}

SYS_RET interrupts_clearhandler(int gate)
{
  // TODO stub
  return SYS_RET_NO_ERROR;
}

SYS_RET interrupts_disablegate(int gate)
{
  // TODO stub
  return arch_interrupts_disablegate(gate);
}

SYS_RET interrupts_disableinterrupts()
{
  return arch_interrupts_disableinterrupts();
}

void interrupts_generic_handler(uint32_t int_number, uint32_t err_code, void *frame)
{
  if(intHandlers[int_number]) {
    (*(intHandlers[int_number]))(int_number, err_code, frame);
  } else {
    interrupts_disableinterrupts();
    tty_puts(tty_getdefaulthandle(), "An unexpected exception has occured. System halted.\n", 53);
    while(TRUE);
  }
  while(TRUE);
}
