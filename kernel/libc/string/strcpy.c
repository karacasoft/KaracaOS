#include <libc/string.h>

char *kaos_strcpy(char *dest, const char *source) {
  size_t size = kaos_strlen(source);
  kaos_memcpy(dest, source, size);
  *(dest + size) = '\0';
  return dest + size;
}

char *kaos_strncpy(char *dest, const char *source, size_t n) {
  size_t size = kaos_strnlen(source, n);
  kaos_memcpy(dest, source, size);
  if(size == n) size--;
  *(dest + size) = '\0';
  return dest + size;
}
