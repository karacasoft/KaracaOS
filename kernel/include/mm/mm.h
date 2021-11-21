#ifndef __MM_H__
#define __MM_H__ 1

#include <stddef.h>
#include <stdint.h>
#include <sysdefs.h>

// Physical memory regions
#define PAGE_TABLES_START     0x00500000U
#define PAGE_TABLES_END       0x00A00000U

#define USER_MEM_SPACE_START  0x01000000U

// Virtual memory regions
#define KERNEL_MEM_START      0xC0000000U


/* Page allocation bitmap */
extern uint8_t mm_page_alloc_bitmap[131072];
extern uint8_t mm_vm_alloc_bitmap[131072];

#define ALLOC_PAGE(x) (mm_page_alloc_bitmap[(x) / 8] |= (1 << (x % 8)))
#define UNALLOC_PAGE(x) (mm_page_alloc_bitmap[(x) / 8] &= ~(1 << (x % 8)))
#define ISALLOCATED_PAGE(x) (mm_page_alloc_bitmap[(x) / 8] & (x % 8))

#define VM_ALLOC_PAGE(x) (mm_vm_alloc_bitmap[(x) / 8] |= (1 << (x % 8)))
#define VM_UNALLOC_PAGE(x) (mm_vm_alloc_bitmap[(x) / 8] &= ~(1 << (x % 8)))
#define VM_ISALLOCATED_PAGE(x) (mm_vm_alloc_bitmap[(x) / 8] & (x % 8))

SYS_RET mm_init();
SYS_RET mm_alloc(void **ret, uint32_t vaddr, uint32_t size, uint32_t flags);
SYS_RET mm_unalloc(void *ret);

void *kaos_kernmalloc(size_t size);

#endif // __MM_H__