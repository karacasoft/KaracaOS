#include <kernel/sysdefs.h>
#include <libc/stdio.h>
#include <libc/string.h>
#include <stddef.h>
#include <stdint.h>


int kaos_fprintf(FILE *stream, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int retVal = kaos_vfprintf(stream, format, args);
  va_end(args);
  return retVal;
}

// TODO: no floating point support and no wide char support
// TODO: Specs require any conversion to be able to convert at least 4095 bytes
int kaos_vfprintf(FILE *stream, const char *format, va_list args) {
  char buffer[4096];
  kaos_memset(buffer, 0, 4096);

  int retVal = kaos_vsprintf(buffer, format, args);
  kaos_fwrite(buffer, 1, retVal, stream);

  return retVal;
}
