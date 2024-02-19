#include "kernel/sysdefs.h"
#include <elf/elf.h>
#include "libc/string.h"

SYS_RET elf__parse_header(Elf32_Ehdr *ret, void *buffer) {
  unsigned char *bufchr = (unsigned char *) buffer;
  if(!(bufchr[EI_MAG0] == ELFMAG0 && bufchr[EI_MAG1] == ELFMAG1 && bufchr[EI_MAG2] == ELFMAG2 && bufchr[EI_MAG3] == ELFMAG3)) {
    return SYS_RET_MALFORMED;
  }
  kaos_memcpy((void *) ret, buffer, sizeof(Elf32_Ehdr));
  return SYS_RET_NO_ERROR;
}

SYS_RET elf__parse_section_header(Elf32_Shdr, void *buffer) {

  return SYS_RET_NO_ERROR;
}
