#include <arch/interrupts/irq.h>
#include <syscall/syscall.h>
#include <libc/stdio.h>

SYS_RET syscall__generic_syscall_handler(enum syscall_function function, uint32_t params[5]) {
  SYS_RET ret = SYS_RET_NO_ERROR;

  if (function == SYS_CALL_FOPEN) {
    kaos_printf("fopen syscall called\n");
  } else if (function == SYS_CALL_FREAD) {
    kaos_printf("fread syscall called\n");
  } else if (function == SYS_CALL_FCLOSE) {
    kaos_printf("fclose syscall called\n");
  } else if (function == SYS_CALL_FWRITE) {
    kaos_printf("fwrite syscall called\n");
  } else {
    kaos_printf("unknown syscall\n");
    kaos_printf("0x%x\n", params);
  }

  return ret;
}
