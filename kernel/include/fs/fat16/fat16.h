#include "dev/block/block_dev.h"
#ifndef __FAT16_H__
#define __FAT16_H__ 1

#include "kernel/sysdefs.h"
#include <stdint.h>
#include <stddef.h>

#define FAT_ATTRIB_READ_ONLY (1 << 0)
#define FAT_ATTRIB_HIDDEN (1 << 1)
#define FAT_ATTRIB_SYSTEM (1 << 2)
#define FAT_ATTRIB_VOLUME_LABEL (1 << 3)
#define FAT_ATTRIB_SUBDIR (1 << 4)
#define FAT_ATTRIB_ARCHIVE (1 << 5)
#define FAT_ATTRIB_RESERVED1 (1 << 6)
#define FAT_ATTRIB_RESERVED2 (1 << 7)

#define CLUSTER_OF(dir_entry)                                                  \
  ((((uint32_t)(dir_entry).starting_cluster_high) << 16) |                     \
   ((uint32_t)(dir_entry).starting_cluster_low))

typedef struct __fat16_boot_sector fat16_boot_sector_t;
typedef struct __fat16_context fat16_context_t;
typedef struct __fat16_dir_entry fat16_dir_entry_t;

struct __fat16_boot_sector {
  uint8_t jmpCode[3];
  uint8_t oemId[8];
  uint16_t bytesPerSector;
  uint8_t sectorsPerCluster;
  uint16_t reservedSectors;
  uint8_t nrOfFATCopies;
  uint16_t nrPossibleRootEntries;
  uint16_t nrSectorsSmall;
  uint8_t mediaDescriptor;
  uint16_t sectorsPerFAT;
  uint16_t sectorsPerTrack;
  uint16_t nrHeads;
  uint32_t hiddenSectors;
  uint32_t nrSectorsLarge;
  uint8_t driveNumber;
  uint8_t reserved0;
  uint8_t extendedBootSignature;
  uint8_t volumeSerialNumber[4];
  char volumeLabel[11];
  char fileSystemType[8];
  uint8_t code[448];
  uint16_t bootSignature; // 0x55AA
} __attribute__((packed));

struct __fat16_dir_entry {
  char filename[8];
  char extension[3];
  uint8_t attributes;
  uint8_t reserved; // always 0
  uint8_t creation_time;
  uint16_t creation_time2;
  uint16_t creation_date;
  uint16_t last_access_date;
  uint16_t starting_cluster_high; // fat32 only
  uint16_t last_modified_time;
  uint16_t last_modified_date;
  uint16_t starting_cluster_low;
  uint32_t file_size;
} __attribute__((packed));

struct __fat16_context {
  block_device_t *device;
  fat16_boot_sector_t boot_sector;
  uint32_t fat_start;
  uint32_t root_dir_start;
  uint32_t data_area_start_cluster;

  void *buffer;
};

SYS_RET fat16_create_context(block_device_t *device,
                             fat16_context_t *out_context);

SYS_RET fat16_read_root_dir(fat16_context_t *context,
                            fat16_dir_entry_t *dir_entries,
                            uint32_t max_entries,
                            uint32_t cluster_offset);

SYS_RET fat16_read_directory(fat16_context_t *context, uint32_t cluster,
                             fat16_dir_entry_t *out_dir_entries,
                             uint32_t max_entries);

SYS_RET fat16_find_file_entry(fat16_context_t *context, char *path,
                              fat16_dir_entry_t *out_entry);

SYS_RET fat16_read_file(fat16_context_t *context, fat16_dir_entry_t *entry,
                        void *buffer, size_t len, size_t *out_len);

#endif // __FAT16_H__
