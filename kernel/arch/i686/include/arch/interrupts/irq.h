#ifndef __IRQ_H__
#define __IRQ_H__ 1

#include <stdint.h>
#include <kernel/sysdefs.h>
#include <syscall/syscall.h>

#include <interrupts/interrupts.h>

typedef SYS_RET (*syscall_handler_t)(enum syscall_function, uint32_t params[5]);

void _arch_irq_generic_handler(uint32_t intNo, generic_int_handler_t handler);

SYS_RET arch_irq_enable(uint16_t gate);
SYS_RET arch_irq_disable(uint16_t gate);
SYS_RET arch_irq_initialize();

int_return_t __arch__syscall_initial_handler(uint32_t intNo, uint32_t errCode, void *stackFrame);
SYS_RET arch_set_syscall_handler(syscall_handler_t);

#endif
