#ifndef __MM_H__
#define __MM_H__ 1

#include <kernel/sysdefs.h>
#include <multiboot.h>
#include <stddef.h>
#include <stdint.h>

// Physical memory regions
#define PAGE_TABLES_START 0x00500000U
#define PAGE_TABLES_END 0x00A00000U

#define USER_MEM_SPACE_START 0x01000000U

// Virtual memory regions
#define KERNEL_MEM_START 0xC0000000U

#define ALLOC_PAGE(x) (mm_page_alloc_bitmap[(x) / 8] |= (1 << (x % 8)))
#define UNALLOC_PAGE(x) (mm_page_alloc_bitmap[(x) / 8] &= ~(1 << (x % 8)))
#define ISALLOCATED_PAGE(x) (mm_page_alloc_bitmap[(x) / 8] & (1 << (x % 8)))

#define VM_ALLOC_PAGE(x) (mm_vm_alloc_bitmap[(x) / 8] |= (1 << (x % 8)))
#define VM_UNALLOC_PAGE(x) (mm_vm_alloc_bitmap[(x) / 8] &= ~(1 << (x % 8)))
#define VM_ISALLOCATED_PAGE(x) (mm_vm_alloc_bitmap[(x) / 8] & (1 << (x % 8)))

#define MM_ALLOC_MEMORY_INFO_FLAGS_PRESENT 0x1

typedef struct __allocated_memory_info mm_allocated_memory_info_t;
typedef struct __reserved_memory_info mm_reserved_memory_info_t;

struct __allocated_memory_info {
  uint32_t flags;
  uint64_t virt_addr;
  uint64_t phys_addr;
  uint64_t size;
};

struct __reserved_memory_info {
  uint64_t virt_addr;
  uint64_t phys_addr;
  uint64_t size;
};

SYS_RET
mm_init(multiboot_info_t *info);
SYS_RET mm_find_empty_block_physical(void **ret_addr);
SYS_RET mm_alloc(void **ret, uint32_t vaddr, uint32_t size, uint32_t flags);
SYS_RET mm_free(void *ret);
BOOLEAN mm_is_memory_available(uint32_t size, uint32_t *vm_block);

void *kaos_kernmalloc(size_t size);

#endif // __MM_H__
