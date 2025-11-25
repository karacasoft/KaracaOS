#include <syscall/syscall.h>
#include <interrupts/interrupts.h>
#include <kernel/sysdefs.h>
#include <arch/iobase.h>
#include <arch/interrupts/irq.h>

#define IRQ_TIMER 		0x00
#define IRQ_KEYBOARD		0x01
#define IRQ_CASCADE		0x02 // Cascaded into chip 2 (never raised)
#define IRQ_SERIAL_PORT_2	0x03 // Shared with serial port 4, if present
#define IRQ_SERIAL_PORT		0x04 // Shared with serial port 3, if present
#define IRQ_PARALLEL_PORT_2	0x05
#define IRQ_FLOPPY		0x06
#define IRQ_PARALLEL_PORT	0x07

#define IRQ_CLOCK		0x08
#define IRQ_ACPI		0x09

#define IRQ_MOUSE		0x12
#define IRQ_COPROCESSOR		0x13
#define IRQ_PRIMARY_ATA_CH	0x14
#define IRQ_SECONDARY_ATA_CH	0x15

#define PIC1 0x20
#define PIC2 0xA0

#define PIC1_COMMAND PIC1
#define PIC2_COMMAND PIC2

#define PIC1_DATA (PIC1+1)
#define PIC2_DATA (PIC2+1)

extern void prepareInterruptGate(uint8_t gate, uint32_t handler, uint16_t selector, uint8_t flags);

void setup_pic(uint8_t pic1offset, uint8_t pic2offset);

void setup_pic(uint8_t pic1offset, uint8_t pic2offset)
{
  unsigned char m1, m2;

	m1 = ~0x02;
	m2 = ~0x80;

	outb(PIC1_COMMAND, 0x11);
	outb(PIC2_COMMAND, 0x11);

	outb(PIC1_DATA, pic1offset);
	outb(PIC2_DATA, pic2offset);

	outb(PIC1_DATA, 4);
	outb(PIC2_DATA, 2);

	outb(PIC1_DATA, 0x01);
	outb(PIC2_DATA, 0x01);

	outb(PIC1_DATA, m1);
  outb(PIC2_DATA, m2);
}

void _arch_irq_generic_handler(uint32_t intNo, generic_int_handler_t handler)
{
  (*handler)(intNo, 0, 0x0);
  if(intNo > 39) outb(PIC2_COMMAND, 0x20);
  outb(PIC1_COMMAND, 0x20);
}

SYS_RET arch_irq_enable(uint16_t gate)
{
  if(gate < 8) {
    uint8_t mask = inb(PIC1_DATA);
    outb(PIC1_DATA, mask & ~(1 << (gate)));
    return SYS_RET_NO_ERROR;
  } else if(gate < 16) {
    uint8_t mask = inb(PIC2_DATA);
    outb(PIC2_DATA, mask & ~(1 << (gate - 8)));
    return SYS_RET_NO_ERROR;
  } else {
    return SYS_RET_OUT_OF_RANGE;
  }
}

SYS_RET arch_irq_disable(uint16_t gate)
{
  if(gate < 8) {
    uint8_t mask = inb(PIC1_DATA);
    outb(PIC1_DATA, mask | (1 << (gate)));
    return SYS_RET_NO_ERROR;
  } else if(gate < 16) {
    uint8_t mask = inb(PIC2_DATA);
    outb(PIC2_DATA, mask | (1 << (gate - 8)));
    return SYS_RET_NO_ERROR;
  } else {
    return SYS_RET_OUT_OF_RANGE;
  }
}

SYS_RET arch_irq_initialize()
{
  setup_pic(32, 40);
  return SYS_RET_NO_ERROR;
}



