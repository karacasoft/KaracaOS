#include <interrupts/interrupts.h>

extern SYS_RET arch_interrupts_enableinterrupts();

extern SYS_RET arch_interrupts_enablegate(int gate);
extern SYS_RET arch_interrupts_sethandler(int gate, int_handler_t handler);

extern SYS_RET arch_interrupts_clearhandler(int gate);
extern SYS_RET arch_interrupts_disablegate(int gate);

extern SYS_RET arch_interrupts_disableinterrupts();

extern SYS_RET arch_interrupts_init();


SYS_RET interrupts_init()
{
    return arch_interrupts_init();
}

SYS_RET interrupts_enableinterrupts()
{
    return arch_interrupts_enableinterrupts();
}

SYS_RET interrupts_enablegate(int gate)
{
    return arch_interrupts_enablegate(gate);
}

SYS_RET interrupts_sethandler(int gate, int_handler_t handler)
{
    return arch_interrupts_sethandler(gate, handler);
}

SYS_RET interrupts_clearhandler(int gate)
{
    return arch_interrupts_clearhandler(gate);
}

SYS_RET interrupts_disablegate(int gate)
{
    return arch_interrupts_disablegate(gate);
}

SYS_RET interrupts_disableinterrupts()
{
    return arch_interrupts_disableinterrupts();
}
