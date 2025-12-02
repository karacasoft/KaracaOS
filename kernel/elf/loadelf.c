#include <kernel/sysdefs.h>
#include <elf/elf.h>
#include <libc/stdio.h>
#include <libc/string.h>
#include <mm/buddy_allocator.h>


// TODO!!! for dynamic linking, later
SYS_RET elf__process_relocation(void *buffer, size_t size, Elf32_Shdr *shdr) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  uint32_t idx;

  if (shdr->sh_type == ET_REL) {
    for (idx = 0; idx < shdr->sh_size / shdr->sh_entsize; idx++) {
      Elf32_Rel *reltab = &((Elf32_Rel *)((void *)buffer + shdr->sh_offset))[idx];

    }
  }


  return ret;
}


SYS_RET elf__load_elf_file(void *buffer, size_t size, void **loaded_loc, void **entry) {
  SYS_RET ret = SYS_RET_NO_ERROR;
  int i;

  Elf32_Ehdr hdr;
  ret = elf__parse_header(&hdr, buffer);
  if (ret) {
    kaos_printf("ELF header parse failed");
  }

  kaos_printf("Entry is here: 0x%x\n", hdr.e_entry);

  Elf32_Off phoff = hdr.e_phoff;
  Elf32_Half phnum = hdr.e_phnum;
  kaos_printf("PH offset is 0x%x\n", phoff);
  kaos_printf("PH count: %d\n", phnum);

  Elf32_Phdr phdrs[100];
  kaos_memset(phdrs, 0, 100);
  ret = elf__parse_program_headers(phdrs, &hdr, buffer);
  if (ret) {
    kaos_printf("Failed to parse PH headers");
  }

  uint32_t mem_low = 0;
  uint32_t mem_high = 0;

  Elf32_Phdr phdr;
  for (i = 0; i < phnum; i++) {
    phdr = phdrs[i];

    uint32_t phdr_mem_high = phdr.p_vaddr + phdr.p_memsz;
    if (phdr_mem_high > mem_high) mem_high = phdr_mem_high;
  }

  void *loc;
  ret = buddy_allocator__alloc(mem_high - mem_low, &loc);
  kaos_printf("Loaded at: %#x\n", loc);
  if (ret) {
    kaos_printf("Memory alloc error");
    return ret;
  }
  *entry = loc + hdr.e_entry;

  for (i = 0; i < phnum; i++) {
    phdr = phdrs[i];

    if (phdr.p_type == 1) {
      kaos_memcpy(loc + phdr.p_vaddr, &buffer[phdr.p_offset], phdr.p_filesz);
    }
  }

  *loaded_loc = loc;

  return ret;
}
