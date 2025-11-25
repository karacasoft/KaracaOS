#include "dev/block/block_dev.h"
#include <interrupts/interrupts.h>
#include <kernel/sysdefs.h>
#include <kernel/test.h>

#include <fs/ext2/ext2.h>
#include <fs/fat16/fat16.h>

#include <dev/ide/ide.h>
#include <dev/pci/pci.h>
#include <tty/tty.h>

#include <libc/stdio.h>
#include <libc/string.h>

#include <elf/elf.h>
#include <mm/buddy_allocator.h>
#include <mm/mm.h>

#include <stdint.h>
#include <elf/elf.h>

int format_ide_device();
int read_fat16();

int my_main() {
  // const char *hello_world = "Hello world!";
  // uint8_t *hello;
  // kaos_printf("Allocating memory...\n");
  // SYS_RET ret = buddy_allocator__alloc(13, (void **)&hello);
  // kaos_printf("Allocated addr: 0x%x\n", hello);
  // kaos_printf("Allocating memory 2\n");
  // uint8_t *deneme;
  // SYS_RET ret2 = buddy_allocator__alloc(25, (void **)&deneme);
  // kaos_printf("Allocated addr: 0x%x\n", deneme);
  // if(ret != SYS_RET_NO_ERROR) {
  //   kaos_printf("Buddy Alloc error\n");
  // }
  // if(ret2 != SYS_RET_NO_ERROR) {
  //   kaos_printf("Buddy alloc error 2\n");
  // }
  //
  // size_t i;
  // for(i = 0; i < 13; i++) {
  //   hello[i] = hello_world[i];
  //   deneme[i] = hello_world[i];
  // }
  //
  // kaos_printf("%s\n", hello);
  // buddy_allocator__free(hello);
  // kaos_printf("deneme %s\n", deneme);
  // kaos_printf("Hello? %s\n", hello);
  //
  // read_fat16();

  const char *filename = "stdout";
  asm volatile ("int $0x80\n" : : "a"(1), "D"(filename));

  return 0;
}

int file_system_traverser() {}

int read_fat16() {

  // fat16_dir_entry_t dir_entry;
  // char *file_contents;
  // SYS_RET ret = buddy_allocator__alloc(4096 * 3, (void **)&file_contents);
  // if (ret != SYS_RET_NO_ERROR) {
  //   kaos_printf("Error");
  //   return ret;
  // }
  //
  // fat16_context_t context;
  // fat16_create_context(&block_device, &context);
  // fat16_find_file_entry(&context, "/EXAMPLE.ELF", &dir_entry);
  // fat16_read_file(&context, &dir_entry, file_contents, 6000);
  //
  // Elf32_Ehdr hdr;
  // ret = elf__parse_header(&hdr, file_contents);
  // if (ret) {
  //   kaos_printf("ELF header parse failed");
  // }
  //
  // kaos_printf("Entry is here: 0x%x\n", hdr.e_entry);
  //
  // Elf32_Off phoff = hdr.e_phoff;
  // Elf32_Half phnum = hdr.e_phnum;
  // kaos_printf("PH offset is 0x%x\n", phoff);
  // kaos_printf("PH count: %d\n", phnum);
  //
  // Elf32_Phdr phdrs[100] = {0};
  // ret = elf__parse_program_headers(phdrs, &hdr, file_contents);
  // if (ret) {
  //   kaos_printf("Failed to parse PH headers");
  // }
  //
  // Elf32_Phdr phdr;
  // for (i = 0; i < phnum; i++) {
  //   phdr = phdrs[i];
  //
  //   kaos_printf("PHDR %d\n", i);
  //   kaos_printf("  Type: %d\n", phdr.p_type);
  //   kaos_printf("  Offset: 0x%x\n", phdr.p_offset);
  //   kaos_printf("  V. Addr: 0x%x\n", phdr.p_vaddr);
  //   kaos_printf("  P. Addr: 0x%x\n", phdr.p_paddr);
  //   kaos_printf("  File Sz.: 0x%x\n", phdr.p_filesz);
  //   kaos_printf("  Mem Sz.: 0x%x\n", phdr.p_memsz);
  //   kaos_printf("  Flags: 0x%x\n", phdr.p_flags);
  //   kaos_printf("  Align: %d\n", phdr.p_align);
  //
  //   if (phdr.p_type == 1) {
  //     void *loc;
  //     kaos_printf("Allocating memory for ELF\n");
  //     ret = buddy_allocator__alloc(phdr.p_memsz, &loc);
  //     if (ret) {
  //       kaos_printf("Mem alloc error\n");
  //       return 1;
  //     }
  //     kaos_printf("Allocated location: 0x%x\n", loc);
  //
  //     kaos_printf("Copying data to location\n");
  //     kaos_memcpy(loc, &file_contents[phdr.p_offset], phdr.p_filesz);
  //
  //     void *entry = loc + hdr.e_entry - phdr.p_vaddr;
  //
  //     int (*entry_func)(void) = entry;
  //
  //     int ret_val = entry_func();
  //
  //     kaos_printf("Loaded program returned %d\n", ret_val);
  //
  //   }
  // }

  return 0;
}
