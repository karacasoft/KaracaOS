#ifndef __BLOCK_DEV_H__
#define __BLOCK_DEV_H__ 1

#include <kernel/sysdefs.h>
#include <dev/ide/ide.h>
#include <stdint.h>

typedef struct __block_device block_device_t;

/**
 * Structure for a block device identifier
 */
struct __block_device {
    uint32_t sectorSize;
    uint32_t size_in_kb;
    ide_device_t *ide_dev;
    void *rw_buffer;
};

/**
 * Reads a sector from block device. Writes returned bytes into buffer.
 * 
 * @param sectorNum LBA address of sector to read
 * @param buffer buffer to write disk data into
 */
SYS_RET block_dev_read(block_device_t *device, uint64_t sectorNum, void *buffer);

/**
 * Writes contents of a buffer into a sector in block device.
 * 
 * @param sectorNum LBA address of sector to write data into
 * @param buffer buffer to write into disk
 */
SYS_RET block_dev_write(block_device_t *device, uint64_t sectorNum, void *buffer);

#endif // __BLOCK_DEV_H__