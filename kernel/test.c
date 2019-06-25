#include <kernel/test.h>
#include <kernel/sysdefs.h>

#include <fs/ext2/ext2.h>

#include <dev/pci/pci.h>
#include <dev/ide/ide.h>
#include <tty/tty.h>

#include <libc/stdio.h>
#include <libc/string.h>

#include <mm/mm.h>

#include <stdint.h>

int my_main() {
    ide_ctrl_t *ide_ctrl;
    for(int bus = 0; bus < 256; bus++) {
        for(int slot = 0; slot < 32; slot++) {
            if(pci_checkDeviceExists(bus, slot)) {
                kaos_printf("Device detected %d:%d\n", bus, slot);
                for(int func = 0; func < 8; func++) {
                    if(pci_checkFunctionExists(bus, slot, func)) {
                        kaos_printf("Function %d:", func);
                        kaos_printf("\tClassCode: %x | SubClassCode: %x\n", pci_getClassCode(bus, slot, func), pci_getSubclassCode(bus, slot, func));
                        if(pci_getClassCode(bus, slot, func) == 0x01 &&
                                pci_getSubclassCode(bus, slot, func) == 0x01) {
                            if(pci_getProgInterface(bus, slot, func) == 0x80) {
                                uint16_t BAR4 = pci_configReadWord(bus, slot, func, 0x20);
                                ide_ctrl = ide_ctrlinit(0x1F0, 0x3F6, 0x170, 0x376, BAR4);
                            }
                        }

                    }
                }
            }
        }
    }

    ide_device_t *device;
    int i;
    for(i = 0; i < 4; i++) {
        if(ide_ctrl->devices[i].deviceExists) {
            device = &(ide_ctrl->devices[i]);
            break;
        }
    }

    block_device_t block_device;

    block_device.nrSectors = device->capacityKb;
    block_device.sectorSize = 512;
    block_device.ide_dev = device;

    kaos_printf("Formatting device\n");
    ext2_format_device(&block_device);
    kaos_printf("Device formatted\n");
    
    return 0;
}