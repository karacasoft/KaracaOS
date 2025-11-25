

char *strchr(const char *str, int ch) {
  const char *ptr = str;
  while(*ptr != '\0') {
    if (*ptr == ch) {
      return ptr;
    }
    ptr++;
  }
  return 0;
}
