SYS_RET arch_interrupts_init()
{
    
}

SYS_RET arch_interrupts_enableinterrupts()
{
    asm volatile ("sti");
}

SYS_RET arch_interrupts_enablegate(int gate)
{

}

SYS_RET arch_interrupts_sethandler(int gate, int_handler_t handler)
{

}

SYS_RET arch_interrupts_clearhandler(int gate)
{

}

SYS_RET arch_interrupts_disablegate(int gate)
{

}

SYS_RET arch_interrupts_disableinterrupts()
{
    asm volatile ("cli");
}

