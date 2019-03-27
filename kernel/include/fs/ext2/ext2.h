#ifndef __EXT2_H__
#define __EXT2_H__ 1

#include <stdint.h>
#include <stddef.h>
#include <kernel/sysdefs.h>
#include <dev/block/block_dev.h>

#define EXT2_FS_STATE_CLEAN                   1
#define EXT2_FS_STATE_ERROR                   2

#define EXT2_ON_ERROR_IGNORE                  1
#define EXT2_ON_ERROR_REMOUNT_AS_READONLY     2
#define EXT2_ON_ERROR_KERNEL_PANIC            3

#define EXT2_OS_ID_LINUX                      0
#define EXT2_OS_ID_GNU_HURD                   1
#define EXT2_OS_ID_MASIX                      2
#define EXT2_OS_ID_FREEBSD                    3
#define EXT2_OS_ID_OTHER_LITES                4


typedef struct __ext2_block_group_descriptor ext2_block_group_descriptor_t;
typedef struct __ext2_superblock ext2_superblock_t;
typedef struct __ext2_inode ext2_inode_t;


struct __ext2_inode {
    uint16_t typeAndPermissions;
    uint16_t uid;
    uint32_t fileSizeLow;
    uint32_t lastAccessTime;
    uint32_t creationTime;
    uint32_t lastModifyTime;
    uint32_t deletionTime;
    uint16_t gid;
    uint16_t nrHardLinks;
    uint32_t nrDiskSectorsUsed;
    uint32_t flags;
    uint32_t osSpecific;
    uint32_t directBlockP0;
    uint32_t directBlockP1;
    uint32_t directBlockP2;
    uint32_t directBlockP3;
    uint32_t directBlockP4;
    uint32_t directBlockP5;
    uint32_t directBlockP6;
    uint32_t directBlockP7;
    uint32_t directBlockP8;
    uint32_t directBlockP9;
    uint32_t directBlockP10;
    uint32_t directBlockP11;
    uint32_t singlyIndirectPointer;
    uint32_t doublyIndirectPointer;
    uint32_t triplyIndirectPointer;
    uint32_t generationNumber;
    uint32_t extendedAttrBlock;
    uint32_t fileSizeHigh;
    uint32_t addrFragment;
    uint32_t osSpecific2[3];
} __attribute__((packed));


struct __ext2_block_group_descriptor {
    uint32_t addrBlockUsageBitmap;
    uint32_t addrInodeUsageBitmap;
    uint32_t addrInodeTable;
    uint16_t nrUnallocatedBlocks;
    uint16_t nrUnallocatedInodes;
    uint16_t nrDirectoriesInGroup;
    uint16_t unused[7];
} __attribute__((packed));

struct __ext2_superblock {
    uint32_t nrInodes;
    uint32_t nrBlocks;
    uint32_t nrBlocksReservedForSU;
    uint32_t nrUnallocatedBlocks;
    uint32_t nrUnallocatedInodes;
    uint32_t superblockNumber;
    uint32_t log2blocksizeminus10;
    uint32_t log2fragmentsizeminus10;
    uint32_t nrBlocksPerBlockGroup;
    uint32_t nrFragmentsPerBlockGroup;
    uint32_t nrInodesPerBlockGroup;
    uint32_t lastMountTime;
    uint32_t lastWrittenTime;
    uint16_t nrMountsAfterFsck;
    uint16_t nrMountsAllowedBeforeFsck;
    uint16_t ext2Signature; // 0xEF53
    uint16_t fsState;
    uint16_t onError;
    uint16_t versionMinor;
    uint32_t lastFsckTime;
    uint32_t fsckInterval;
    uint32_t osId;
    uint32_t versionMajor;
    uint16_t uidThatCanUseReservedBlocks;
    uint16_t gidThatCanUseReservedBlocks;

    // extended fields
    uint32_t firstAvailableInode;
    uint16_t sizeOfInodeInBytes;
    uint16_t blockGroupId;
    uint32_t optionalFeatures;
    uint32_t requiredFeatures;
    uint32_t requiredFeaturesForWriteAccess;
    char fsId[16];
    char volumeName[16];
    char lastMountPath[64];
    uint32_t compressionAlgorithms;
    uint8_t nrBlocksToPreallocateFiles;
    uint8_t nrBlocksToPreallocateDirectories;
    uint16_t unusued0;
    char journalId[16];
    uint32_t journalInode;
    uint32_t journalDevice;
    uint32_t headOfOrphanInodeList;

    // Unused other bytes... 788
} __attribute__((packed));

SYS_RET ext2_format_device(block_device_t *blockDevice);
SYS_RET ext2_mount_device(block_device_t *blockDevice);
//SYS_RET ext2_unmount_device(block_device_t *blockDevice);

SYS_RET ext2_get_file(uint8_t *retfd, char *filename);
SYS_RET ext2_read_file(uint8_t fd, void *buffer, size_t bufferSize);
SYS_RET ext2_write_file(uint8_t fd, void *buffer, size_t bufferSize);


#endif // __EXT2_H__