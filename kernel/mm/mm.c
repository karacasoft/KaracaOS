#include <libc/stdio.h>
#include <libc/string.h>
#include <mm/mm.h>

extern void heap_start;
void *heap_ptr = &heap_start;

uint8_t mm_page_alloc_bitmap[131072] = {0};
uint8_t mm_vm_alloc_bitmap[131072] = {0};

uint32_t mm_total_memory_block_count = 0;
uint32_t mm_available_block_count = 0;

uint8_t mm_mem_info_idx = 0;
mm_allocated_memory_info_t mm_memory_info[256];

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
  for (i = 0; i < size; i += sizeof(multiboot_memory_map_t)) {
    multiboot_memory_map_t *entry =
        (multiboot_memory_map_t *)(info->mmap_addr + i);

    if (entry->type == MULTIBOOT_MEMORY_AVAILABLE) {
      kaos_printf("%d: Addr: %x Len: %x\n", j++, (uint32_t)entry->addr,
                  (uint32_t)entry->len);
      uint32_t start_block = (entry->addr / 4096);
      uint32_t block_count = (entry->len / 4096);

      uint32_t k;

      mm_page_alloc_bitmap[start_block / 8] = ~(0xFFU << (start_block % 8));

      for (k = (start_block + 8) & (~0x7); k <= block_count; k += 8) {
        mm_page_alloc_bitmap[k / 8] = 0;
      }

      mm_page_alloc_bitmap[(k - 8) / 8] = 0xFF << (block_count % 8);

      total_block_count += block_count;
    }
  }
  // TODO not accurate, calculate later
  mm_available_block_count = total_block_count - 4096;
  mm_total_memory_block_count = mm_available_block_count;
  kaos_printf("Total 4KB blocks: %d\n", total_block_count);

  for (i = 0; i < 4096 / 8; i++) {
    mm_page_alloc_bitmap[i] = 0xFF;
  }

  for (i = 98304; i < 131072; i++) {
    mm_vm_alloc_bitmap[i] = 0xFF;
  }
  return SYS_RET_NO_ERROR;
}

BOOLEAN mm_is_memory_available(uint32_t size, uint32_t *vm_block) {
  uint16_t count = (size / 4096) + 1;

  if (mm_available_block_count < count)
    return FALSE;

  uint32_t free_vm_blocks = 0;
  uint32_t start_vm_block = *vm_block;
  uint32_t start;
  for (start = start_vm_block; start < 98304 * 8; start++) {
    if (mm_vm_alloc_bitmap[start / 8] == 0xFF)
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
  if (VM_ISALLOCATED_PAGE(vaddr / 4096)) {
    return SYS_RET_MEM_ALLOC_ERROR;
  }

  uint32_t vm_block = vaddr / 4096;
  if (mm_is_memory_available(size, &vm_block)) {
    while (mm_memory_info[mm_mem_info_idx].flags &
           MM_ALLOC_MEMORY_INFO_FLAGS_PRESENT) {
      mm_mem_info_idx++;
    }

    uint32_t i;
    for (i = vm_block; i < vm_block + size_in_blocks; i++) {
      uint32_t j = USER_MEM_SPACE_START / 4096;
      while ((mm_page_alloc_bitmap[j / 8] >> (j % 8)) & 0x1 == 1) {
        j++;
      }
      ret = arch_paging_map(i * 4096, j * 4096);
      if (ret != SYS_RET_NO_ERROR) {
        return ret;
      }
      mm_page_alloc_bitmap[j / 8] |= (1 << (j % 8));
      mm_vm_alloc_bitmap[i / 8] |= (1 << (i % 8));
    }

    *ret_addr = (void *)(vm_block * 4096);

    mm_memory_info[mm_mem_info_idx].flags |= MM_ALLOC_MEMORY_INFO_FLAGS_PRESENT;
    mm_memory_info[mm_mem_info_idx].virt_addr = vm_block * 4096;
    mm_memory_info[mm_mem_info_idx].size = size;
    mm_mem_info_idx++;
    return SYS_RET_NO_ERROR;
  } else {
    return SYS_RET_MEM_ALLOC_ERROR;
  }
}

SYS_RET mm_free(void *addr) {
  uint16_t i;
  for (i = 0; i < 256; i++) {
    if (mm_memory_info[i].flags & MM_ALLOC_MEMORY_INFO_FLAGS_PRESENT) {
      if ((uint32_t)addr == mm_memory_info[i].virt_addr) {
        uint64_t size_in_blocks = mm_memory_info[i].size / 4096;
        mm_memory_info[i].flags &= ~(0x1);

        uint32_t vm_block = (uint32_t)addr / 4096;

        uint32_t j;
        for (j = vm_block; j < vm_block + size_in_blocks; j++) {
          uint32_t phys_addr;
          arch_paging_virt_to_phys(&phys_addr, j * 4096);
          uint32_t phys_block = phys_addr / 4096;

          arch_paging_unmap(j * 4096);

          mm_page_alloc_bitmap[phys_block / 8] &= ~(1 << (phys_block % 8));
          mm_vm_alloc_bitmap[j / 8] &= ~(1 << (j % 8));
        }
      }
    }
  }
  return SYS_RET_NO_ERROR;
}
