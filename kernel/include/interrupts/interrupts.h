#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__ 1

#include <kernel/sysdefs.h>
#include <stdint.h>

typedef enum __int_return {
  INT_RETURN_NO_ERROR = 0,
  INT_RETURN_UNIDENTIFIED_ERROR,
  INT_RETURN_WRONG_HANDLER
} int_return_t;

typedef int_return_t (*int_handler_t)(uint32_t intNo, uint32_t errCode, void *fullStackFrame);

typedef SYS_RET (*generic_int_handler_t)(uint32_t intNo, uint32_t errCode, void *fullStackFrame);

SYS_RET interrupts_init();

SYS_RET interrupts_enableinterrupts();

SYS_RET interrupts_enablegate(int gate);
SYS_RET interrupts_sethandler(int gate, int_handler_t handler);

SYS_RET interrupts_clearhandler(int gate);
SYS_RET interrupts_disablegate(int gate);

SYS_RET interrupts_disableinterrupts();

#endif
