#include <stdio.h>
#include <stdarg.h>

int printf(const char *format, ...) {
  va_list args;
  va_start(args, format);
  int retVal = vfprintf(stdout, format, args);
  va_end(args);
  return retVal;
}
