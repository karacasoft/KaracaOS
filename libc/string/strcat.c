#include <string.h>

char *strcat(char *dest, const char *src) {
  char *ptr = dest;
  const char *ptr2 = src;
  while(*ptr != '\0') {
    ptr++;
  }

  while(*ptr2 != '\0') {
    *ptr = *ptr2;
    ptr++;
    ptr2++;
  }
  *ptr = '\0';

  return dest;
}
