#include <kernel/sysdefs.h>
#include <interrupts/interrupts.h>

#include <arch/interrupts/irq.h>

extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

extern void _isr32();
extern void _isr33();
extern void _isr34();
extern void _isr35();
extern void _isr36();
extern void _isr37();
extern void _isr38();
extern void _isr39();
extern void _isr40();
extern void _isr41();
extern void _isr42();
extern void _isr43();
extern void _isr44();
extern void _isr45();
extern void _isr46();
extern void _isr47();

extern void _isr128();

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

typedef struct __X86InterruptStackFrame {
  uint16_t gs, fs, es, ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t int_no;
  uint32_t err_code;
} X86InterruptStackFrame;

static generic_int_handler_t intHandler = 0;

static syscall_handler_t generic_syscall_handler = 0;

static IDTInfo idtInfo;
static IDTEntry idtEntries[INTERRUPT_VECTOR_COUNT];

void prepareInterruptGate(uint8_t gate, uint32_t handler, uint16_t selector, uint8_t flags) {
  idtEntries[gate].offset1 = (uint16_t)(handler & 0xFFFF);
  idtEntries[gate].offset2 = (uint16_t)(handler >> 16);
  idtEntries[gate].selector = selector;
  idtEntries[gate].flags = flags;
  idtEntries[gate].reserved = 0;
}

void lidt(void *base, uint16_t size) {
  idtInfo.size = size;
  idtInfo.offset = (uint32_t) base;

  asm volatile ("lidt %0" :  : "m"(idtInfo) );
}

SYS_RET arch_interrupts_init()
{
  prepareInterruptGate(0 , (uint32_t) _isr0 , 0x10, 0x8E);
  prepareInterruptGate(1 , (uint32_t) _isr1 , 0x10, 0x8E);
  prepareInterruptGate(2 , (uint32_t) _isr2 , 0x10, 0x8E);
  prepareInterruptGate(3 , (uint32_t) _isr3 , 0x10, 0x8E);
  prepareInterruptGate(4 , (uint32_t) _isr4 , 0x10, 0x8E);
  prepareInterruptGate(5 , (uint32_t) _isr5 , 0x10, 0x8E);
  prepareInterruptGate(6 , (uint32_t) _isr6 , 0x10, 0x8E);
  prepareInterruptGate(7 , (uint32_t) _isr7 , 0x10, 0x8E);
  prepareInterruptGate(8 , (uint32_t) _isr8 , 0x10, 0x8E);
  prepareInterruptGate(9 , (uint32_t) _isr9 , 0x10, 0x8E);
  prepareInterruptGate(10, (uint32_t) _isr10, 0x10, 0x8E);
  prepareInterruptGate(11, (uint32_t) _isr11, 0x10, 0x8E);
  prepareInterruptGate(12, (uint32_t) _isr12, 0x10, 0x8E);
  prepareInterruptGate(13, (uint32_t) _isr13, 0x10, 0x8E);
  prepareInterruptGate(14, (uint32_t) _isr14, 0x10, 0x8E);
  prepareInterruptGate(15, (uint32_t) _isr15, 0x10, 0x8E);
  prepareInterruptGate(16, (uint32_t) _isr16, 0x10, 0x8E);
  prepareInterruptGate(17, (uint32_t) _isr17, 0x10, 0x8E);
  prepareInterruptGate(18, (uint32_t) _isr18, 0x10, 0x8E);
  prepareInterruptGate(19, (uint32_t) _isr19, 0x10, 0x8E);
  prepareInterruptGate(20, (uint32_t) _isr20, 0x10, 0x8E);
  prepareInterruptGate(21, (uint32_t) _isr21, 0x10, 0x8E);
  prepareInterruptGate(22, (uint32_t) _isr22, 0x10, 0x8E);
  prepareInterruptGate(23, (uint32_t) _isr23, 0x10, 0x8E);
  prepareInterruptGate(24, (uint32_t) _isr24, 0x10, 0x8E);
  prepareInterruptGate(25, (uint32_t) _isr25, 0x10, 0x8E);
  prepareInterruptGate(26, (uint32_t) _isr26, 0x10, 0x8E);
  prepareInterruptGate(27, (uint32_t) _isr27, 0x10, 0x8E);
  prepareInterruptGate(28, (uint32_t) _isr28, 0x10, 0x8E);
  prepareInterruptGate(29, (uint32_t) _isr29, 0x10, 0x8E);
  prepareInterruptGate(30, (uint32_t) _isr30, 0x10, 0x8E);
  prepareInterruptGate(31, (uint32_t) _isr31, 0x10, 0x8E);

  prepareInterruptGate(32, (uint32_t) _isr32, 0x10, 0x8E);
  prepareInterruptGate(33, (uint32_t) _isr33, 0x10, 0x8E);
  prepareInterruptGate(34, (uint32_t) _isr34, 0x10, 0x8E);
  prepareInterruptGate(35, (uint32_t) _isr35, 0x10, 0x8E);
  prepareInterruptGate(36, (uint32_t) _isr36, 0x10, 0x8E);
  prepareInterruptGate(37, (uint32_t) _isr37, 0x10, 0x8E);
  prepareInterruptGate(38, (uint32_t) _isr38, 0x10, 0x8E);
  prepareInterruptGate(39, (uint32_t) _isr39, 0x10, 0x8E);
  prepareInterruptGate(40, (uint32_t) _isr40, 0x10, 0x8E);
  prepareInterruptGate(41, (uint32_t) _isr41, 0x10, 0x8E);
  prepareInterruptGate(42, (uint32_t) _isr42, 0x10, 0x8E);
  prepareInterruptGate(43, (uint32_t) _isr43, 0x10, 0x8E);
  prepareInterruptGate(44, (uint32_t) _isr44, 0x10, 0x8E);
  prepareInterruptGate(45, (uint32_t) _isr45, 0x10, 0x8E);
  prepareInterruptGate(46, (uint32_t) _isr46, 0x10, 0x8E);
  prepareInterruptGate(47, (uint32_t) _isr47, 0x10, 0x8E);
  prepareInterruptGate(128, (uint32_t) _isr128, 0x10, 0x8E);

  SYS_RET ret = arch_irq_initialize();
  if(ret) return ret;

  lidt(&idtEntries, INTERRUPT_VECTOR_COUNT * (sizeof(IDTEntry)) - 1);
  return SYS_RET_NO_ERROR;
}

SYS_RET arch_interrupts_enableinterrupts()
{
    asm volatile ("sti");
    return SYS_RET_NO_ERROR;
}

SYS_RET arch_interrupts_enablegate(int gate)
{
  if(gate >= 32 && gate < 48) {
    arch_irq_enable(gate - 32);
  }
  return SYS_RET_NO_ERROR;
}

SYS_RET arch_interrupts_setgenerichandler(generic_int_handler_t handler)
{
  intHandler = handler;
  return SYS_RET_NO_ERROR;
}

SYS_RET arch_interrupts_disablegate(int gate)
{
  if(gate >= 32 && gate < 48) {
    arch_irq_disable(gate - 32);
  }
  return SYS_RET_NO_ERROR;
}

SYS_RET arch_interrupts_disableinterrupts()
{
    asm volatile ("cli");
    return SYS_RET_NO_ERROR;
}

void _arch_interrupt_handler(X86InterruptStackFrame *frame)
{
  if(frame->int_no > 31 && frame->int_no < 48) {
    _arch_irq_generic_handler(frame->int_no, intHandler);
  } else {
    intHandler(frame->int_no, frame->err_code, (void *)frame);
  }

}

int_return_t __arch__syscall_initial_handler(uint32_t intNo, uint32_t errCode, void *stackFrame) {
  X86InterruptStackFrame *frame = stackFrame;
  // arch specific code: 32-bit architecture, warnings ok
  uint32_t syscall_num = frame->eax;
  void *param1 = (void *) frame->edi;
  void *param2 = (void *) frame->esi;
  void *param3 = (void *) frame->ebx;
  void *param4 = (void *) frame->ecx;
  void *param5 = (void *) frame->edx;
  
  (*generic_syscall_handler)(syscall_num, (void *[]) {param1, param2, param3, param4, param5});
  return INT_RETURN_NO_ERROR;
}

SYS_RET arch_set_syscall_handler(syscall_handler_t handler) {
  generic_syscall_handler = handler;
  return SYS_RET_NO_ERROR;
}
