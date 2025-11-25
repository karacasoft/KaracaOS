#include "kernel/sysdefs.h"
#include "libc/string.h"
#include <elf/elf.h>

SYS_RET elf__parse_header(Elf32_Ehdr *ret, void *buffer) {
  unsigned char *bufchr = (unsigned char *)buffer;
  if (!(bufchr[EI_MAG0] == ELFMAG0 && bufchr[EI_MAG1] == ELFMAG1 &&
        bufchr[EI_MAG2] == ELFMAG2 && bufchr[EI_MAG3] == ELFMAG3)) {
    return SYS_RET_MALFORMED;
  }

  uint8_t class = bufchr[EI_CLASS];
  if (class == ELFCLASS64 || class == ELFCLASSNONE) {
    return SYS_RET_UNSUPPORTED;
  }

  uint8_t data = bufchr[EI_DATA];
  if (data == ELFDATA2MSB || data == ELFDATANONE) {
    return SYS_RET_UNSUPPORTED;
  }

  kaos_memcpy((void *)ret, buffer, sizeof(Elf32_Ehdr));
  return SYS_RET_NO_ERROR;
}

SYS_RET elf__parse_section_headers(Elf32_Shdr *out_shdr, Elf32_Ehdr *elf_hdr,
                                  void *buffer) {
  uint32_t shdr_offset = elf_hdr->e_shoff;
  uint32_t shdr_len = elf_hdr->e_shentsize;
  uint32_t num_ent = elf_hdr->e_shnum;

  void *shtable = buffer + shdr_offset;
  size_t i;
  for(i = 0; i < num_ent; i++) {
    kaos_memcpy(&out_shdr[i], shtable + i * shdr_len, shdr_len);
  }

  return SYS_RET_NO_ERROR;
}

SYS_RET elf__parse_program_headers(Elf32_Phdr *out_phdr, Elf32_Ehdr *elf_hdr,
                                   void *buffer) {
  uint32_t phdr_offset = elf_hdr->e_phoff;
  uint32_t phdr_len = elf_hdr->e_phentsize;
  uint32_t num_ent = elf_hdr->e_phnum;

  void *phtable = buffer + phdr_offset;
  size_t i;
  for(i = 0; i < num_ent; i++) {
    kaos_memcpy(&out_phdr[i], phtable + i * phdr_len, phdr_len);
  }

  return SYS_RET_NO_ERROR;
}
