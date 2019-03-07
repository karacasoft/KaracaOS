#include <kernel/test.h>

#include <dev/pci/pci.h>
#include <tty/tty.h>

#include <libc/stdio.h>

#include <stdint.h>

int my_main() {
    tty_handle_t *tty = tty_getdefaulthandle();

    for(int bus = 0; bus < 256; bus++) {
        for(int slot = 0; slot < 32; slot++) {
            if(pci_checkDeviceExists(bus, slot)) {
                kaos_printf("Device detected %d:%d\n", bus, slot);
                kaos_printf("Vendor: %x | Device: %x\n", pci_getVendorId(bus, slot), pci_getDeviceId(bus, slot));
            }
        }
    }
    return 0;
}