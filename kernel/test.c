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

  SYS_RET ret = SYS_RET_NO_ERROR;
  kaos_printf("Loading elf file\n");
  FILE *fp = kaos_fopen("/EXAMPLE.ELF", "r");
  char *file_contents;
  ret = buddy_allocator__alloc(1024 * 30, (void **)&file_contents);
  if (ret) {
    kaos_printf("Mem allocation error\n");
    return ret;
  }
  int file_size = kaos_fread(file_contents, 1, 1024 * 30, fp);

  void *loaded_location;
  void *entry;
  ret = elf__load_elf_file(file_contents, file_size, &loaded_location, &entry);
  if (ret) {
    kaos_printf("Elf load error\n");
    return ret;
  }
  
  void (*entry_func)(void) = entry;

  entry_func();

  return 0;
}

int file_system_traverser() {
  return -1;
}

