#include <stdio.h>

size_t fread(void *buffer, size_t size, size_t count, FILE *stream) {
  size_t retVal;
  asm volatile("int $0x80" : : "a"(2), "D"(buffer), "S"(size), "b"(count), "c"(stream), "d"(&retVal));
  return retVal;
}

