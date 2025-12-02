#include <arch/interrupts/irq.h>
#include <syscall/syscall.h>
#include <libc/stdio.h>

SYS_RET syscall__handle_fopen(void *params[5]) {
    FILE **fpp = (FILE **) params[0];
    const char *path = (const char *) params[1];
    const char *mode = (const char *) params[2];
    *fpp = kaos_fopen(path, mode);

    return SYS_RET_NO_ERROR;
}


SYS_RET syscall__handle_fread(void *params[5]) {
  void *outBuffer = params[0];
  uint32_t size = (uint32_t) params[1];
  uint32_t count = (uint32_t) params[2];
  FILE *fp = (FILE *) params[3];
  size_t *retVal = params[4];

  *retVal = kaos_fread(outBuffer, size, count, fp);

  return SYS_RET_NO_ERROR;
}


SYS_RET syscall__handle_fwrite(void *params[5]) {
  void *buffer = params[0];
  uint32_t size = (uint32_t) params[1];
  uint32_t count = (uint32_t) params[2];
  FILE *fp = (FILE *) params[3];
  size_t *retVal = params[4];

  *retVal = kaos_fwrite(buffer, size, count, fp);

  return SYS_RET_NO_ERROR;
}


SYS_RET syscall__generic_syscall_handler(enum syscall_function function, void *params[5]) {
  SYS_RET ret = SYS_RET_NO_ERROR;

  if (function == SYS_CALL_FOPEN) {
    kaos_printf("fopen syscall called\n");
    ret = syscall__handle_fopen(params);
  } else if (function == SYS_CALL_FREAD) {
    kaos_printf("fread syscall called\n");
    ret = syscall__handle_fread(params);
  } else if (function == SYS_CALL_FCLOSE) {
    kaos_printf("fclose syscall called\n");
  } else if (function == SYS_CALL_FWRITE) {
    kaos_printf("fwrite syscall called\n");
    ret = syscall__handle_fwrite(params);
  } else {
    kaos_printf("unknown syscall\n");
    kaos_printf("0x%x\n", params);
  }

  return ret;
}
