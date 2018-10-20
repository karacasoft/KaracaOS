#include <kernel/sysdefs.h>

typedef struct __IDTEntry {
  uint16_t offset1;
  uint16_t selector;
  uint8_t reserved;
  uint8_t flags;
  uint16_t offset2;
} IDTEntry;

typedef struct __IDTInfo {
  uint16_t size;
  uint32_t offset;
} __attribute__((packed)) IDTInfo;

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
