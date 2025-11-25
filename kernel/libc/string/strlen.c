#include <libc/string.h>

size_t kaos_strlen(const char *s) {
  size_t count = 0;
  while(*s++ != '\0') {
    count++;
  }
  return count;
}

size_t kaos_strnlen(const char *s, size_t n) {
  size_t count = 0;
  while(n--) {
    if(*s == '\0') return count;
    count++;
  }
  return count;
}
