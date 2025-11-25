#ifndef __PCI_H__
#define __PCI_H__

#include <stdint.h>

#include <kernel/sysdefs.h>

typedef struct __pci_config_type0 {
    uint16_t deviceId;
    uint16_t vendorId;
    uint16_t status;
    uint16_t command;
    uint8_t classCode;
    uint8_t subClassCode;
    uint8_t progIF;
    uint8_t revId;
    uint8_t builtInSelfTest;
    uint8_t headerType;
    uint8_t latencyTimer;
    uint8_t cacheLineSize;
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t carbusCISPointer;
    uint16_t subsystemId;
    uint16_t subsystemVendorId;
    uint32_t expansionRomBaseAddress;
    uint8_t RESERVED0[3];
    uint8_t capabilitiesPointer;
    uint32_t RESERVED1;
    uint8_t maxLatency;
    uint8_t minGrant;
    uint8_t interruptPin;
    uint8_t interruptLine;
} pci_config_type0_t;

uint16_t pci_configReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t pci_configReadDWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

uint16_t pci_getVendorId(uint8_t bus, uint8_t slot, uint8_t func);
uint16_t pci_getDeviceId(uint8_t bus, uint8_t slot, uint8_t func);

uint16_t pci_getStatus(uint8_t bus, uint8_t slot, uint8_t func);

uint8_t pci_getClassCode(uint8_t bus, uint8_t slot, uint8_t func);

uint8_t pci_getSubclassCode(uint8_t bus, uint8_t slot, uint8_t func);

uint8_t pci_getProgInterface(uint8_t bus, uint8_t slot, uint8_t func);

BOOLEAN pci_checkDeviceExists(uint8_t bus, uint8_t slot);
BOOLEAN pci_checkFunctionExists(uint8_t bus, uint8_t slot, uint8_t func);

#endif // __PCI_H__
