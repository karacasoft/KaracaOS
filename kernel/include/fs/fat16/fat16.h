#ifndef __FAT16_H__ 
#define __FAT16_H__ 1

#include <stdint.h>

typedef struct __fat16_boot_sector fat16_boot_sector_t;

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

#endif // __FAT16_H__
