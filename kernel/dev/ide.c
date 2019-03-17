#include <dev/ide/ide.h>
#include <dev/ide/ata.h>

#include <libc/stdio.h>

#define INT_SWAP(x,y) { x = x + y; y = x - y; x = x - y; }

static ide_ctrl_t __ide_ctrl;

extern uint8_t arch_ide_readaltstatus(ide_channel_t *channel);
extern SYS_RET arch_ide_writecmd(ide_channel_t *channel, uint8_t data);
extern SYS_RET arch_ide_writectrl(ide_channel_t *channel, uint8_t offset, uint8_t data);
extern SYS_RET arch_ide_readbuffer(ide_channel_t *channel, void *buffer, size_t len);
extern SYS_RET arch_ide_writebuffer(ide_channel_t *channel, void *buffer, size_t len);

void fill_device_info(ide_device_t *device, ide_channel_t *channel, ata_drive_identify_buffer_t *identifyInfo);
SYS_RET wait_for_drive_ready(ide_channel_t *channel);


SYS_RET ide_identify_drive(ide_channel_t *channel, ata_drive_identify_buffer_t *buffer) {
    uint8_t status;

    ide_writectrl(channel, ATA_REG_COMMAND_OFFSET, ATA_CMD_IDENTIFY);
    SYS_RET ret = wait_for_drive_ready(channel);
    if(ret != SYS_RET_NO_ERROR) return ret;

    arch_ide_readbuffer(channel, (void *) buffer, 512);
    return SYS_RET_NO_ERROR;
}

ide_ctrl_t *ide_ctrlinit(uint16_t BAR0, uint16_t BAR1, uint16_t BAR2, uint16_t BAR3, uint16_t BAR4) {
    __ide_ctrl.channels[0].cmdRegsBase = BAR0;
    __ide_ctrl.channels[0].ctrlReg = BAR1;
    __ide_ctrl.channels[1].cmdRegsBase = BAR2;
    __ide_ctrl.channels[1].ctrlReg = BAR3;

    __ide_ctrl.channels[0].bmide = BAR4;
    __ide_ctrl.channels[1].bmide = BAR4 + 8;

    for(int i = 0; i < 4; i++) {
        __ide_ctrl.devices[i].deviceId = i % 2;
        __ide_ctrl.devices[i].deviceExists = FALSE;
    }


    ide_writectrl(&(__ide_ctrl.channels[0]), ATA_REG_DRIVEHEAD_OFFSET, 0xA0);
    for(int i=0; i < 1000000; i++); // use interrupts god damn it...

    ata_drive_identify_buffer_t buffer;
    if(ide_identify_drive(&(__ide_ctrl.channels[0]), &buffer) == SYS_RET_NO_ERROR) {
        fill_device_info(&(__ide_ctrl.devices[0]), &(__ide_ctrl.channels[0]), &buffer);
    }
    

    ide_writectrl(&(__ide_ctrl.channels[0]), ATA_REG_DRIVEHEAD_OFFSET, 0xB0);
    for(int i=0; i < 1000000; i++); // use interrupts god damn it...

    if(ide_identify_drive(&(__ide_ctrl.channels[0]), &buffer) == SYS_RET_NO_ERROR) {
        fill_device_info(&(__ide_ctrl.devices[1]), &(__ide_ctrl.channels[0]), &buffer);
    }



    ide_writectrl(&(__ide_ctrl.channels[1]), ATA_REG_DRIVEHEAD_OFFSET, 0xA0);
    for(int i=0; i < 1000000; i++); // use interrupts god damn it...

    if(ide_identify_drive(&(__ide_ctrl.channels[1]), &buffer) == SYS_RET_NO_ERROR) {
        fill_device_info(&(__ide_ctrl.devices[2]), &(__ide_ctrl.channels[1]), &buffer);
    }
    

    ide_writectrl(&(__ide_ctrl.channels[1]), ATA_REG_DRIVEHEAD_OFFSET, 0xB0);
    for(int i=0; i < 1000000; i++); // use interrupts god damn it...

    if(ide_identify_drive(&(__ide_ctrl.channels[1]), &buffer) == SYS_RET_NO_ERROR) {
        fill_device_info(&(__ide_ctrl.devices[3]), &(__ide_ctrl.channels[1]), &buffer);
    }
    
    

    return &__ide_ctrl;
}

ide_ctrl_t *ide_getdefaultctrl() {
    return &__ide_ctrl;
}

SYS_RET ide_writecmd(ide_channel_t *channel, uint8_t data) {
    return arch_ide_writecmd(channel, data);
}

SYS_RET ide_writectrl(ide_channel_t *channel, uint8_t offset, uint8_t data) {
    return arch_ide_writectrl(channel, offset, data);
}

uint8_t ide_readaltstatus(ide_channel_t *channel) {
    return arch_ide_readaltstatus(channel);
}

SYS_RET ide_readsector(ide_device_t *device, void *buffer, uint32_t lba, uint32_t numSectors) {
    if(!device->deviceExists) return SYS_RET_NO_DEVICE;
    
    ide_writectrl(device->channel, ATA_REG_SECCOUNT_OFFSET, (uint8_t) numSectors);
    ide_writectrl(device->channel, ATA_REG_LBA0_7_OFFSET, ((lba >> 0) & 0xFF));
    ide_writectrl(device->channel, ATA_REG_LBA8_15_OFFSET, ((lba >> 8) & 0xFF));
    ide_writectrl(device->channel, ATA_REG_LBA16_23_OFFSET, ((lba >> 16) & 0xFF));
    ide_writectrl(device->channel, ATA_REG_LBA24_27_OFFSET, (((lba >> 24) & 0xFF) & (device->deviceId << 4)) | (0b11100000));

    ide_writectrl(device->channel, ATA_REG_COMMAND_OFFSET, ATA_CMD_READ);

    SYS_RET ret = wait_for_drive_ready(device->channel);
    if(ret != SYS_RET_NO_ERROR) return ret;

    arch_ide_readbuffer(device->channel, buffer, numSectors * 512);

    return SYS_RET_NO_ERROR;
}

SYS_RET ide_writesector(ide_device_t *device, void *buffer, uint32_t lba, uint32_t numSectors) {
    if(!device->deviceExists) return SYS_RET_NO_DEVICE;
    
    ide_writectrl(device->channel, ATA_REG_SECCOUNT_OFFSET, (uint8_t) numSectors);
    ide_writectrl(device->channel, ATA_REG_LBA0_7_OFFSET, ((lba >> 0) & 0xFF));
    ide_writectrl(device->channel, ATA_REG_LBA8_15_OFFSET, ((lba >> 8) & 0xFF));
    ide_writectrl(device->channel, ATA_REG_LBA16_23_OFFSET, ((lba >> 16) & 0xFF));
    ide_writectrl(device->channel, ATA_REG_LBA24_27_OFFSET, (((lba >> 24) & 0xFF) & (device->deviceId << 4)) | (0b11100000));

    ide_writectrl(device->channel, ATA_REG_COMMAND_OFFSET, ATA_CMD_WRITE);

    SYS_RET ret = wait_for_drive_ready(device->channel);
    if(ret != SYS_RET_NO_ERROR) return ret;

    arch_ide_writebuffer(device->channel, buffer, numSectors * 512);

    return SYS_RET_NO_ERROR;
}



void fill_device_info(ide_device_t *device, ide_channel_t *channel, ata_drive_identify_buffer_t *identifyInfo) {
    uint64_t sectors = identifyInfo->nrAddressableLogicalSectors;
    uint16_t *sectorsBytes = (uint16_t *) &sectors;
    
    INT_SWAP(sectorsBytes[0], sectorsBytes[1]);
    INT_SWAP(sectorsBytes[2], sectorsBytes[3]);
    
    device->deviceExists = TRUE;
    device->capacityKb = (uint32_t)(sectors / 2);
    device->channel = channel;
    device->lbaSupported = !!(identifyInfo->capabilities0 & ATA_CAPABILITY_LBA_SUPPORTED_MASK);
    device->dmaSupported = !!(identifyInfo->capabilities0 & ATA_CAPABILITY_DMA_SUPPORTED_MASK);
    

    kaos_printf("Found device with capacity %d KB\n", device->capacityKb);
}

SYS_RET wait_for_drive_ready(ide_channel_t *channel) {
    uint8_t status;
    for(int i=0; i < 1000000; i++); // use interrupts god damn it...

    if((status = ide_readaltstatus(channel)) == 0) {
        return SYS_RET_NO_DEVICE;
    }

    while(TRUE) {
        status = ide_readaltstatus(channel);
        if(!(status & ATA_STATUS_BSY_MASK) && (status & ATA_STATUS_DRQ_MASK)) break;
        if(!(status & ATA_STATUS_BSY_MASK) && !(status & ATA_STATUS_DRQ_MASK)) return SYS_RET_UNIDENTIFIED_ERROR;
        if(status & ATA_STATUS_ERR_MASK) return SYS_RET_UNIDENTIFIED_ERROR;
    }

    return SYS_RET_NO_ERROR;
}