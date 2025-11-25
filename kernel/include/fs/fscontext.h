#ifndef __FSCONTEXT_H__
#define __FSCONTEXT_H__ 1

#include <kernel/sysdefs.h>
#include <dev/pci/pci.h>
#include <libc/stdio.h>
#include <dev/block/block_dev.h>
#include <kernel/sysdefs.h>
#include <fs/fat16/fat16.h>

static block_device_t __block_device;
static fat16_context_t __fscontext;
static char __block_device_rwbuffer[4096];

SYS_RET fscontext__init();

#endif
