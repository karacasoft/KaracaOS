#include "dev/block/block_dev.h"
#include <kernel/sysdefs.h>
#include <kernel/test.h>

#include <fs/ext2/ext2.h>
#include <fs/fat16/fat16.h>

#include <dev/ide/ide.h>
#include <dev/pci/pci.h>
#include <tty/tty.h>

#include <libc/stdio.h>
#include <libc/string.h>

#include <mm/mm.h>

#include <stdint.h>

int format_ide_device();

int my_main() {
  format_ide_device();
  return 0;
}

int format_ide_device() {
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

  block_device_t block_device;
  char rw_buffer[4096] = {0};

  block_device.size_in_kb = device->capacityKb;
  block_device.sectorSize = 512;
  block_device.ide_dev = device;
  block_device.rw_buffer = rw_buffer;

  fat16_dir_entry_t dir_entry;
  char *file_contents;
  SYS_RET ret = mm_alloc((void **)&file_contents, 0x30000000, 4096 * 3, 0);
  if(ret != SYS_RET_NO_ERROR) {
    kaos_printf("Error");
    return ret;
  }

  fat16_context_t context;
  fat16_create_context(&block_device, &context);
  fat16_find_file_entry(&context, "/VAYBE/TEST100.TXT", &dir_entry);
  fat16_read_file(&context, &dir_entry, file_contents, 6000);

  kaos_printf("File contents: %s\n", file_contents);
  kaos_printf("EOF?? 0x%x\n", file_contents[4]);

  return 0;
}
