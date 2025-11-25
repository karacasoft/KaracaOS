#include <dev/pci/pci.h>
#include <kernel/sysdefs.h>

extern uint16_t arch_pci_configReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
extern uint32_t arch_pci_configReadDWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

uint16_t pci_configReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    return arch_pci_configReadWord(bus, slot, func, offset);
}

uint32_t pci_configReadDWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    return arch_pci_configReadDWord(bus, slot, func, offset);
}

uint16_t pci_getVendorId(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_configReadWord(bus, slot, func, 0x0);
}

uint16_t pci_getDeviceId(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_configReadWord(bus, slot, func, 0x2);
}

uint16_t pci_getStatus(uint8_t bus, uint8_t slot, uint8_t func) {
    return pci_configReadWord(bus, slot, func, 0x6);
}

uint8_t pci_getClassCode(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint8_t)(pci_configReadWord(bus, slot, func, 0xA) >> 8);
}

uint8_t pci_getSubclassCode(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint8_t)(pci_configReadWord(bus, slot, func, 0xA) & 0xFF);
}

uint8_t pci_getProgInterface(uint8_t bus, uint8_t slot, uint8_t func) {
    return (uint8_t)(pci_configReadWord(bus, slot, func, 0x8) >> 8);
}

BOOLEAN pci_checkDeviceExists(uint8_t bus, uint8_t slot) {
    return pci_checkFunctionExists(bus, slot, 0);
}

BOOLEAN pci_checkFunctionExists(uint8_t bus, uint8_t slot, uint8_t func) {
    if(pci_getVendorId(bus, slot, func) == 0xFFFF) {
        return FALSE;
    } else {
        return TRUE;
    }
}

