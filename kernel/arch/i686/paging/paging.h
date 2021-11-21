#ifndef __PAGING_H__
#define __PAGING_H__ 1

extern void page_directory_start;

void arch_paging_init_lower() __attribute__((section(".prepaging.text")));
void arch_paging_init_higher();

#endif