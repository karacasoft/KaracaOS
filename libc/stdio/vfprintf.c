#include <kernel/sysdefs.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>


int fprintf(FILE *stream, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int retVal = vfprintf(stream, format, args);
  va_end(args);
  return retVal;
}

// TODO: no floating point support and no wide char support
// TODO: Specs require any conversion to be able to convert at least 4095 bytes
int vfprintf(FILE *stream, const char *format, va_list args) {
  char buffer[4096] = {0};

  int retVal = vsprintf(buffer, format, args);
  fwrite(buffer, 1, retVal, stream);

  return retVal;
}
