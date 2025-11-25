#include <dev/ide/ide.h>
#include <dev/ide/ata.h>

#include <arch/iobase.h>

uint8_t arch_ide_readaltstatus(ide_channel_t *channel);
SYS_RET arch_ide_writecmd(ide_channel_t *channel, uint8_t data);
SYS_RET arch_ide_writectrl(ide_channel_t *channel, uint8_t offset, uint8_t data);
SYS_RET arch_ide_readbuffer(ide_channel_t *channel, void *buffer, size_t len);



uint8_t arch_ide_readaltstatus(ide_channel_t *channel) {
    return inb(channel->ctrlReg);
}

SYS_RET arch_ide_writecmd(ide_channel_t *channel, uint8_t data) {
    outb(channel->ctrlReg, data);
    return SYS_RET_NO_ERROR;
}

SYS_RET arch_ide_readbuffer(ide_channel_t *channel, void *buffer, size_t len) {
    insw(channel->cmdRegsBase + ATA_REG_DATA_OFFSET, buffer, len);
    return SYS_RET_NO_ERROR;
}

SYS_RET arch_ide_writebuffer(ide_channel_t *channel, void *buffer, size_t len) {
    outsw(channel->cmdRegsBase + ATA_REG_DATA_OFFSET, buffer, len);
    return SYS_RET_NO_ERROR;
}

SYS_RET arch_ide_writectrl(ide_channel_t *channel, uint8_t offset, uint8_t data) {
    outb(channel->cmdRegsBase + offset, data);
    return SYS_RET_NO_ERROR;
}

