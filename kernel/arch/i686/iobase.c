#include <stdint.h>
#include "iobase.h"

void outb(uint16_t addr, uint8_t val)
{
	asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(addr) );
}

uint8_t inb(uint16_t addr)
{
	uint8_t ret;
	asm volatile ( "inb %1, %0" : "=a"(ret) : "Nd"(addr) );
	return ret;
}

void outw(uint16_t addr, uint16_t val)
{
	asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(addr) );
}

uint16_t inw(uint16_t addr)
{
	uint16_t ret;
	asm volatile ( "inw %1, %0" : "=a"(ret) : "Nd"(addr) );
	return ret;
}

void outl(uint16_t addr, uint32_t val)
{
	asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(addr) );
}

uint32_t inl(uint16_t addr)
{
	uint32_t ret;
	asm volatile ( "inl %1, %0" : "=a"(ret) : "Nd"(addr) );
	return ret;
}

void insw(uint16_t addr, void *ret, size_t len) {
	len = len / 2;
	asm volatile ("rep insw" :  : "d"(addr), "D"(ret), "c"(len) : "memory" );
}

void insl(uint16_t addr, void *ret, size_t len) {
	len = len / 4;
	asm volatile ("rep insl" :  : "d"(addr), "D"(ret), "c"(len) : "memory" );
}

void outsw(uint16_t addr, void *buf, size_t len) {
	len = len / 2;
	asm volatile ("rep outsw" :  : "d"(addr), "S"(buf), "c"(len) );
}

void outsl(uint16_t addr, void *buf, size_t len) {
	len = len / 4;
	asm volatile ("rep outsl" :  : "d"(addr), "S"(buf), "c"(len) );
}
