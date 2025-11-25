#include <fs/fscontext.h>

SYS_RET fscontext__init() {
  SYS_RET ret = SYS_RET_NO_ERROR;
  ide_ctrl_t *ide_ctrl;
  for (int bus = 0; bus < 256; bus++) {
    for (int slot = 0; slot < 32; slot++) {
      if (pci_checkDeviceExists(bus, slot)) {
        kaos_printf("Device detected %d:%d\n", bus, slot);
        for (int func = 0; func < 8; func++) {
          if (pci_checkFunctionExists(bus, slot, func)) {
            kaos_printf("Function %d:", func);
            kaos_printf("\tClassCode: %x | SubClassCode: %x\n",
                        pci_getClassCode(bus, slot, func),
                        pci_getSubclassCode(bus, slot, func));
            if (pci_getClassCode(bus, slot, func) == 0x01 &&
                pci_getSubclassCode(bus, slot, func) == 0x01) {
              if (pci_getProgInterface(bus, slot, func) == 0x80) {
                uint16_t BAR4 = pci_configReadWord(bus, slot, func, 0x20);
                ide_ctrl = ide_ctrlinit(0x1F0, 0x3F6, 0x170, 0x376, BAR4);
              }
            }
          }
        }
      }
    }
  }

  ide_device_t *device = NULL;
  int i;
  for (i = 0; i < 4; i++) {
    if (ide_ctrl->devices[i].deviceExists) {
      device = &(ide_ctrl->devices[i]);
      break;
    }
  }
 
  if (device == NULL) {
    // panic
    kaos_printf("No block device found, only IDE is supported. Halting...\n");
    return SYS_RET_NO_DEVICE;
  }

  __block_device.size_in_kb = device->capacityKb;
  __block_device.sectorSize = 512;
  __block_device.ide_dev = device;
  __block_device.rw_buffer = __block_device_rwbuffer;
  
  ret = fat16_create_context(&__block_device, &__fscontext);
  if (ret) {
    kaos_printf("FAT16 context creation failed\n");
    return ret;
  }

  
  return ret;
}
