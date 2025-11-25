#include <dev/block/block_dev.h>

SYS_RET block_dev_read(block_device_t *device, uint64_t sectorNum, void *buffer) {
    ide_readsector(device->ide_dev, buffer, sectorNum, 1);
}

SYS_RET block_dev_write(block_device_t *device, uint64_t sectorNum, void *buffer) {
    ide_writesector(device->ide_dev, buffer, sectorNum, 1);
}