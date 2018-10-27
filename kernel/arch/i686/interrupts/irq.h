#ifndef __IRQ_H__
#define __IRQ_H__ 1

#include <stdint.h>
#include <kernel/sysdefs.h>

#include <interrupts/interrupts.h>

void _arch_irq_generic_handler(uint32_t intNo, generic_int_handler_t handler);

SYS_RET arch_irq_enable(uint16_t gate);
SYS_RET arch_irq_disable(uint16_t gate);
SYS_RET arch_irq_initialize();

#endif
