#ifndef __PAGING_H__
#define __PAGING_H__ 1

#include <kernel/sysdefs.h>

extern void page_directory_start;

void arch_paging_init_lower() __attribute__((section(".prepaging.text")));

SYS_RET arch_paging_map(uint32_t virt_addr, uint32_t phys_addr);
SYS_RET arch_paging_unmap(uint32_t virt_addr);

SYS_RET arch_paging_virt_to_phys(uint32_t *phys_addr, uint32_t virt_addr);

#endif