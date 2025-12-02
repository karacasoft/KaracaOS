#include <stdio.h>

FILE *fopen(const char *path, const char *mode) {
  FILE *fp;
  asm volatile ("int $0x80" : : "a"(1), "D"(&fp), "S"(path), "b"(mode) : "memory");
  return fp;
}
