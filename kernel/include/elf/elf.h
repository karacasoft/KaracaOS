#ifndef __ELF_H__
#define __ELF_H__ 1
#include <stdint.h>
#include "kernel/sysdefs.h"

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

#define ET_NONE 0
#define ET_REL  1
#define ET_EXEC 2
#define ET_DYN  3
#define ET_CORE 4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

#define EM_NONE  0
#define EM_M32   1
#define EM_SPARC 2
#define EM_386   3
#define EM_68K   4
#define EM_88K   5
#define EM_860   7
#define EM_MIPS  8
#define EM_MIPS_RS4_BE 10

#define EV_NONE 0
#define EV_CURRENT 1

#define EI_MAG0    0
#define EI_MAG1    1
#define EI_MAG2    2
#define EI_MAG3    3
#define EI_CLASS   4
#define EI_DATA    5
#define EI_VERSION 6
#define EI_PAD     7
#define EI_NIDENT 16

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2

#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;
} Elf32_Ehdr;

#define SHN_UNDEF     0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC    0xff00
#define SHN_HIPROC    0xff1f
#define SHN_ABS       0xfff1
#define SHN_COMMON    0xfff2
#define SHN_HIRESERVE 0xffff

#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_HASH     5
#define SHT_DYNAMIC  6
#define SHT_NOTE     7
#define SHT_NOBITS   8
#define SHT_REL      9
#define SHT_SHLIB   10
#define SHT_DYNSYM  11
#define SHT_LOPROC  0x70000000
#define SHT_HIPROC  0x7fffffff
#define SHT_LOUSER  0x80000000
#define SHT_HIUSER  0xffffffff

#define SHF_WRITE     0x1
#define SHF_ALLOC     0x2
#define SHF_EXECINSTR 0x4
#define SHF_MASKPROC  0xf0000000

typedef struct {
  Elf32_Word sh_name;
  Elf32_Word sh_type;
  Elf32_Word sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off  sh_offset;
  Elf32_Word sh_size;
  Elf32_Word sh_link;
  Elf32_Word sh_addralign;
  Elf32_Word sh_entsize;
} Elf32_Shdr;

SYS_RET elf__parse_header(Elf32_Ehdr *ret, void *buffer);
SYS_RET elf__parse_section_header(Elf32_Shdr, void *buffer);

#endif // !__ELF_H__
