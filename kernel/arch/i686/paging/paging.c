#include "paging.h"
#include <stdint.h>

#define PAGE_SIZE (0x1000)
#define PAGE_TABLE_SIZE (0x1000)
#define PAGE_DIRECTORY_ENTRY_COUNT (1024)
#define PAGE_TABLE_ENTRY_COUNT (1024)

#define KERNEL_PAGE_DIR_INDEX 768

/*  uint32_t present : 1;
    uint32_t read_write : 1;
    uint32_t user_supervisor : 1;
    uint32_t cache_disable : 1;
    uint32_t accessed : 1;
    uint32_t reserved0 : 1;
    uint32_t page_size : 1;
    uint32_t reserved1 : 3;
    uint32_t address : 20;*/
typedef struct __page_directory_entry {
  uint32_t value;
} page_directory_entry_t;

typedef struct __page_table_entry {
  uint32_t value;
  /*    uint32_t present : 1;
      uint32_t read_write : 1;
      uint32_t user_supervisor : 1;
      uint32_t write_through : 1;
      uint32_t cache_disable : 1;
      uint32_t accessed : 1;
      uint32_t dirty : 1;
      uint32_t page_attribute_table : 1;
      uint32_t global : 1;
      uint32_t reserved0 : 3;
      uint32_t address : 20;*/
} page_table_entry_t;

page_directory_entry_t *page_dir __attribute__((section(".prepaging.data"))) =
    (page_directory_entry_t *)(&page_directory_start);
page_table_entry_t *page_tables __attribute__((section(".prepaging.data"))) =
    (page_table_entry_t *)((&page_directory_start) + 0x1000);

static inline void arch_paging_invalidate_single_page(uint32_t addr) {
  asm volatile("invlpg (%0)" : : "r"(addr) : "memory");
}

void arch_paging_init_lower() {
  uint16_t i, j;
  for (i = 0; i < 1024; i++) {
    uint32_t value =
        (1 << 0) | (1 << 1) |
        (((uint32_t)((((void *)page_tables) + (i * PAGE_SIZE)))) >> 12) << 12;

    page_dir[i].value = value;
  }

  uint32_t initial_address = 0x0;
  for (i = 0; i < 1024; i++) {
    for (j = 0; j < 1024; j++) {
      page_tables[i * 1024 + j].value =
          (1 << 1) |
          ((initial_address +
            (i * (PAGE_SIZE * PAGE_TABLE_ENTRY_COUNT) + j * PAGE_SIZE)) >>
           12) << 12;
    }
  }

  for (i = 0; i < 10; i++) {
    for (j = 0; j < 1024; j++) {
      page_tables[i * 1024 + j].value |= (1 << 0);
    }
  }

  page_dir[768].value |= (1 << 0);
  page_dir[769].value |= (1 << 0);
  page_dir[770].value |= (1 << 0);
  page_dir[771].value |= (1 << 0);

  for (i = 0; i < 1024; i++) {
    page_tables[768 * PAGE_TABLE_ENTRY_COUNT + i].value &= 0xFFF;
    page_tables[768 * PAGE_TABLE_ENTRY_COUNT + i].value |=
        (1 << 0) | (0x000000 + i * 0x1000);
  }

  for (i = 0; i < 1024; i++) {
    page_tables[769 * PAGE_TABLE_ENTRY_COUNT + i].value &= 0xFFF;
    page_tables[769 * PAGE_TABLE_ENTRY_COUNT + i].value |=
        (1 << 0) | (0x400000 + i * 0x1000);
  }

  for (i = 0; i < 1024; i++) {
    page_tables[770 * PAGE_TABLE_ENTRY_COUNT + i].value &= 0xFFF;
    page_tables[770 * PAGE_TABLE_ENTRY_COUNT + i].value |=
        (1 << 0) | (0x800000 + i * 0x1000);
  }

  for (i = 0; i < 1024; i++) {
    page_tables[771 * PAGE_TABLE_ENTRY_COUNT + i].value &= 0xFFF;
    page_tables[771 * PAGE_TABLE_ENTRY_COUNT + i].value |=
        (1 << 0) | (0xC00000 + i * 0x1000);
  }

  return;
}

SYS_RET arch_paging_map(uint32_t virt_addr, uint32_t phys_addr) {
  uint64_t table = virt_addr / 4096;
  page_dir[table / 1024].value |= (1 << 0) | (1 << 1);

  page_tables[table].value = 0xFFF;
  page_tables[table].value |= (1 << 0) | (1 << 1) | (phys_addr & ~0xFFF);
  return SYS_RET_NO_ERROR;
}

SYS_RET arch_paging_unmap(uint32_t virt_addr) {
  page_tables[virt_addr / 4096].value = 0;
  arch_paging_invalidate_single_page(virt_addr);
  return SYS_RET_NO_ERROR;
}

SYS_RET arch_paging_virt_to_phys(uint32_t *phys_addr, uint32_t virt_addr) {
  *phys_addr = page_tables[virt_addr / 4096].value & ~0xFFF;
  return SYS_RET_NO_ERROR;
}
