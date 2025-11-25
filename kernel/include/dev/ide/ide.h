#ifndef __IDE_H__
#define __IDE_H__ 1

#include <stdint.h>
#include <kernel/sysdefs.h>

typedef struct __ide_device ide_device_t;
typedef struct __ide_channel ide_channel_t;
typedef struct __ide_ctrl ide_ctrl_t;

struct __ide_device {
    uint8_t deviceId;
    BOOLEAN deviceExists;
    uint32_t capacityKb;
    BOOLEAN dmaSupported;
    BOOLEAN lbaSupported;
    char serialNumber[20];
    char modelNumber[40];

    ide_channel_t *channel;
};

struct __ide_channel {
    uint16_t cmdRegsBase;
    uint16_t ctrlReg;
    uint16_t bmide;
    BOOLEAN noInterrupt;

    ide_ctrl_t *ctrl;
};

struct __ide_ctrl {
    ide_channel_t channels[2];
    ide_device_t devices[4];
};

ide_ctrl_t *ide_ctrlinit(uint16_t BAR0, uint16_t BAR1, uint16_t BAR2, uint16_t BAR3, uint16_t BAR4);
ide_ctrl_t *ide_getdefaultctrl();

uint8_t ide_readaltstatus(ide_channel_t *channel);
SYS_RET ide_writecmd(ide_channel_t *channel, uint8_t data);
SYS_RET ide_writectrl(ide_channel_t *channel, uint8_t offset, uint8_t data);

SYS_RET ide_readsector(ide_device_t *device, void *buffer, uint32_t lba, uint32_t numSectors);
SYS_RET ide_writesector(ide_device_t *device, void *buffer, uint32_t lba, uint32_t numSectors);


#endif // __IDE_H__