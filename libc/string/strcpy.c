#include <string.h>

char *strcpy(char *dest, const char *source) {
  size_t size = strlen(source);
  memcpy(dest, source, size);
  *(dest + size) = '\0';
  return dest + size;
}

char *strncpy(char *dest, const char *source, size_t n) {
  size_t size = strnlen(source, n);
  memcpy(dest, source, size);
  if(size == n) size--;
  *(dest + size) = '\0';
  return dest + size;
}
