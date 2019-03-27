#include <fs/ext2/ext2.h>

void create_superblock(ext2_superblock_t *superblock, block_device_t *device);


SYS_RET ext2_format_device(block_device_t *blockDevice) {
    ext2_superblock_t superblock;
    create_superblock(&superblock, blockDevice);
    
}

SYS_RET ext2_mount_device(block_device_t *blockDevice);
//SYS_RET ext2_unmount_device(block_device_t *blockDevice);

SYS_RET ext2_get_file(uint8_t *retfd, char *filename);
SYS_RET ext2_read_file(uint8_t fd, void *buffer, size_t bufferSize);
SYS_RET ext2_write_file(uint8_t fd, void *buffer, size_t bufferSize);


void create_superblock(ext2_superblock_t *superblock, block_device_t *device) {

}