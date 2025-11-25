#ifndef __BUDDY_ALLOCATOR_H__
#define __BUDDY_ALLOCATOR_H__

#include "kernel/sysdefs.h"
#include <stdint.h>
#include <stddef.h>
#include <multiboot.h>

SYS_RET buddy_allocator__init();
SYS_RET buddy_allocator__alloc(uint32_t len, void **out_addr);
SYS_RET buddy_allocator__free(void *addr);

#endif // __BUDDY_ALLOCATOR_H__
