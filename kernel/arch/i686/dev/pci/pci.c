#include <dev/pci/pci.h>
#include "../../iobase.h"

#include <stdint.h>

uint32_t arch_pci_configReadDWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus = (uint32_t) bus;
    uint32_t lslot = (uint32_t) slot;
    uint32_t lfunc = (uint32_t) func;
    
    address = (uint32_t) ((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xFC) | ((uint32_t) 0x80000000));

    outl(0xCF8, address);

    return inl(0xCFC);
}

uint16_t arch_pci_configReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t res = arch_pci_configReadDWord(bus, slot, func, offset);
    uint16_t tmp = (uint16_t)((res >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}