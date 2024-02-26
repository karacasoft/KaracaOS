#include "kernel/sysdefs.h"
#include "multiboot.h"
#include <libc/stdio.h>
#include <libc/string.h>
#include <mm/mm.h>

extern void heap_start;
void *heap_ptr = &heap_start;

uint8_t mm_page_alloc_bitmap[131072] = {0};
uint8_t mm_vm_alloc_bitmap[131072] = {0};

uint32_t mm_total_memory_block_count = 0;
uint32_t mm_available_block_count = 0;

uint32_t mm_mem_info_idx = 0;
mm_allocated_memory_info_t mm_memory_info[100000];

BOOLEAN mm_is_memory_available(uint32_t size, uint32_t *vm_block);

extern SYS_RET arch_paging_unmap(uint32_t virt_addr);
extern SYS_RET arch_paging_map(uint32_t virt_addr, uint32_t phys_addr);
extern SYS_RET arch_paging_virt_to_phys(uint32_t *phys_addr,
                                        uint32_t virt_addr);

void *kaos_kernmalloc(size_t size) {
  void *curr = heap_ptr;
  kaos_memset(curr, 0, size);
  heap_ptr += size;
  return curr;
}

SYS_RET mm_init(multiboot_info_t *info) {
  uint32_t i;

  if (!((info->flags >> 6) & 0x1)) {
    return SYS_RET_INIT_ERROR;
  }

  for (i = 0; i < 131072; i++) {
    mm_page_alloc_bitmap[i] = 0xFF;
  }

  uint32_t size = info->mmap_length;
  uint32_t total_block_count = 0;

  kaos_printf("Detecting memory...\n");
  int32_t j = 0;
  multiboot_memory_map_t *entry = (multiboot_memory_map_t *)(info->mmap_addr);
  for (i = 0; i < size; i += sizeof(multiboot_memory_map_t)) {

    if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
      kaos_printf("%d: Addr: %x Len: %x\n", j++, (uint32_t)entry->addr,
                  (uint32_t)entry->len);
      uint32_t start_block = (entry->addr / 4096);
      uint32_t block_count = (entry->len / 4096);

      uint32_t k;

      mm_page_alloc_bitmap[start_block / 8] = ~(0xFFU << (start_block % 8));

      for (k = (start_block + 8) & (~0x7); k <= (start_block + block_count);
           k += 8) {
        mm_page_alloc_bitmap[k / 8] = 0;
      }

      mm_page_alloc_bitmap[(k - 8) / 8] = 0xFFU >> (8 - ((start_block + block_count) % 8));

      total_block_count += block_count;
    } else if (entry->type == MULTIBOOT_MEMORY_ACPI_RECLAIMABLE) {
      kaos_printf("ACPI Reclaimable 0x%x - Length: 0x%d\n", entry->addr,
                  entry->len);
    } else if (entry->type == MULTIBOOT_MEMORY_NVS) {
      kaos_printf("NVS 0x%x - Length: 0x%d\n", entry->addr, entry->len);
    } else if (entry->type == MULTIBOOT_MEMORY_BADRAM) {
      kaos_printf("BADRAM 0x%x - Length: 0x%d\n", entry->addr, entry->len);
    } else {
      kaos_printf("Reserved Mem Addr. 0x%x - Length: 0x%d\n", entry->addr,
                  entry->len);
      uint32_t start_block = entry->addr / 4096;
      uint32_t end_block = (entry->addr + entry->len) / 4096;
      if (entry->len > 0) {
        mm_page_alloc_bitmap[start_block / 8] = (0xFFU >> (start_block % 8));

        size_t k;
        for (k = (start_block + 8) & (~0x7); k <= (end_block);
             k += 8) {
          mm_page_alloc_bitmap[k / 8] = 1;
        }

        mm_page_alloc_bitmap[(k - 8) / 8] = 0xFFU >> (end_block % 8);
      }
    }
    entry = (multiboot_memory_map_t *)(((uint8_t *)(entry)) + entry->size + 4);
  }

  for (i = 0; i < 4 * 4096 / 8; i++) {
    mm_page_alloc_bitmap[i] = 0xFFU;
  }

  for (i = 98304; i < 102400; i++) {
    mm_vm_alloc_bitmap[i] = 0xFFU;
  }

  uint32_t available_blocks = 0;
  for(i = 0; i < 131072; i++) {
    if(mm_page_alloc_bitmap[i] == 0x0) {
      available_blocks += 8;
    } else if(mm_page_alloc_bitmap[i] == 0xFFU) {

    } else {
      size_t j;
      for(j = 0; j < 8; j++) {
        if(!(mm_page_alloc_bitmap[i] & (1 << j))) {
          available_blocks += 1;
        }
      }
    }
  }
  mm_available_block_count = available_blocks;
  return SYS_RET_NO_ERROR;
}

SYS_RET mm_find_empty_block_physical(void **ret_addr) {
  uint32_t block_nr = 0;
  while(mm_page_alloc_bitmap[block_nr] == 0xFF) {
    block_nr++;
  }

  size_t i;
  for(i = 0; i < 8; i++) {
    if(!(mm_page_alloc_bitmap[block_nr] & (1 << i))) {
      break;
    }
  }
  if(i == 8) {
    return SYS_RET_NOT_FOUND;
  }

  uint64_t addr = ((block_nr * 8) + i) * 4096;
  *ret_addr = (void *)addr;
  
  return SYS_RET_NO_ERROR;
}

BOOLEAN mm_is_memory_available(uint32_t size, uint32_t *vm_block) {
  uint16_t count = (size / 4096);
  if(count == 0) count = 1;

  if (mm_available_block_count < count) {
    return FALSE;
  }

  uint32_t free_vm_blocks = 0;
  uint32_t start_vm_block = *vm_block;
  uint32_t start;
  for (start = start_vm_block / 8; start < 131072; start++) {
    if (mm_vm_alloc_bitmap[start] == 0xFF)
      continue;
    uint8_t bitmap_piece = mm_vm_alloc_bitmap[start / 8];

    uint8_t i;
    for (i = 0; i < 8; i++) {
      if (bitmap_piece & 0x1)
        free_vm_blocks = 0;
      else {
        if (free_vm_blocks == 0)
          start_vm_block = start + i;
        free_vm_blocks++;
      }
      if (free_vm_blocks == count) {
        *vm_block = start_vm_block;
        return TRUE;
      }
    }
  }

  return FALSE;
}

SYS_RET mm_alloc(void **ret_addr, uint32_t vaddr, uint32_t size,
                 uint32_t flags) {
  SYS_RET ret;
  uint32_t size_in_blocks = size / 4096;
  if (size_in_blocks == 0)
    size_in_blocks = 1;
  uint32_t vm_block = vaddr / 4096;
  if (!(VM_ISALLOCATED_PAGE(vm_block))) {
    void *empty_block;
    mm_find_empty_block_physical(&empty_block);
    uint32_t j = ((uint32_t) empty_block) / 4096;
    ret = arch_paging_map(vm_block * 4096, j * 4096);
    if (ret != SYS_RET_NO_ERROR) {
      kaos_printf("Error while allocating??\n");
      return ret;
    }
    ALLOC_PAGE(j);
    VM_ALLOC_PAGE(vm_block);

    *ret_addr = (void *)(vm_block * 4096);

    return SYS_RET_NO_ERROR;
  } else {
    kaos_printf("VM address already allocated\n");
    return SYS_RET_MEM_ALLOC_ERROR;
  }
}

SYS_RET mm_free(void *addr) {
  uint16_t i;
  uint32_t vm_block = (uint32_t)addr / 4096;

  uint32_t phys_addr;
  arch_paging_virt_to_phys(&phys_addr, vm_block * 4096);
  uint32_t phys_block = phys_addr / 4096;

  arch_paging_unmap(phys_addr);

  mm_page_alloc_bitmap[phys_block / 8] &= ~(1 << (phys_block % 8));
  mm_vm_alloc_bitmap[vm_block / 8] &= ~(1 << (vm_block % 8));
  return SYS_RET_NO_ERROR;
}
