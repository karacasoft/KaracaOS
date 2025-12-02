#ifndef __SYSCALL_H__
#define __SYSCALL_H__ 1

#include <kernel/sysdefs.h>

#define SYSCALL_INT_NUM 0x80

enum syscall_function {
  SYS_CALL_FOPEN = 1,
  SYS_CALL_FREAD = 2,
  SYS_CALL_FWRITE = 3,
  SYS_CALL_FCLOSE = 4,
};

SYS_RET syscall__generic_syscall_handler(enum syscall_function function, void *params[5]);

#endif
