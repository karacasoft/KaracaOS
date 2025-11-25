#include <libc/string.h>

size_t strlen(const char *s) {
  size_t count = 0;
  while(*s++ != '\0') {
    count++;
  }
  return count;
}

size_t strnlen(const char *s, size_t n) {
  size_t count = 0;
  while(n--) {
    if(*s == '\0') return count;
    count++;
  }
  return count;
}
