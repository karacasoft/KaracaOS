#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__ 1

#include <kernel/sysdefs.h>
#include <stdint.h>

typedef int int_return_t;

typedef int_return_t (*int_handler_t)(void *params);

SYS_RET interrupts_init();

SYS_RET interrupts_enableinterrupts();

SYS_RET interrupts_enablegate(int gate);
SYS_RET interrupts_sethandler(int gate, int_handler_t handler);

SYS_RET interrupts_clearhandler(int gate);
SYS_RET interrupts_disablegate(int gate);

SYS_RET interrupts_disableinterrupts();

#endif