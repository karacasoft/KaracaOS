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
    unsigned char *mem;
    mm_alloc((void **) &mem, 0xA0000000, 4096, 0);

    const char hello[] = "Hello world";

    uint32_t i;
    for(i = 0; i < sizeof(hello); i++) {
        mem[i] = hello[i];
    }
    mem[i] = '\0';
    
    kaos_printf("%s", mem);

    mm_free(mem);

    kaos_printf("%s", mem);

    return 0;
}

int format_ide_device() {
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
    char rw_buffer[4096] = {0};

    block_device.size_in_kb = device->capacityKb;
    block_device.sectorSize = 512;
    block_device.ide_dev = device;
    block_device.rw_buffer = rw_buffer;

    kaos_printf("Formatting device\n");
    ext2_format_device(&block_device);
    kaos_printf("Device formatted\n");

    return 0;
}