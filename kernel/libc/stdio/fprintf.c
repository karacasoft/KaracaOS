#include "kernel/sysdefs.h"
#include <libc/stdio.h>

#define FORMAT_STATE_FLAGS 1
#define FORMAT_STATE_FIELD_WIDTH 2
#define FORMAT_STATE_PRECISION 3
#define FORMAT_STATE_LENGTH 4
#define FORMAT_STATE_SPECIFIER 5

#define FORMAT_FLAG_LEFT_JUSTIFY 0x1
#define FORMAT_FLAG_BEGIN_WITH_SIGN 0x2
#define FORMAT_FLAG_PAD_NO_SIGN 0x4
#define FORMAT_FLAG_ALT_FORM 0x8
#define FORMAT_FLAG_PAD_WITH_ZEROES 0x10

int fprintf(FILE *stream, const char* format, ...) {
  va_list args;
  va_start(args, format);

  char buffer[4096] = {0};
  char *bufferPtr = buffer;

  const char *formatPtr = format;
  while (*formatPtr != '\0') {
    if (*formatPtr == '%') {
      unsigned char state = FORMAT_STATE_FLAGS;
      unsigned int flags = 0;
      int fieldWidth = 0;
      int precision = 0;
      unsigned int length = 0;
      unsigned int specifier = 0;
      BOOLEAN formatFinished = FALSE;
      while(!formatFinished) {
        formatPtr++;
        if (state == FORMAT_STATE_FLAGS) {
          switch (*formatPtr) {
            case '%':
              *bufferPtr++ = '%';
              formatFinished = TRUE;
              break;
            case '-':
              flags |= FORMAT_FLAG_LEFT_JUSTIFY;
              break;
            case '+':
              flags |= FORMAT_FLAG_BEGIN_WITH_SIGN;
              break;
            case ' ':
              flags |= FORMAT_FLAG_PAD_NO_SIGN;
              break;
            case '#':
              flags |= FORMAT_FLAG_ALT_FORM;
              break;
            case '0':
              flags |= FORMAT_FLAG_PAD_WITH_ZEROES;
              break;
            default:
              state = FORMAT_STATE_FIELD_WIDTH;
              break;
          }
        }

        if (state == FORMAT_STATE_FIELD_WIDTH) {
          if ((*formatPtr) >= '0' && (*formatPtr) <= '9') {
            fieldWidth *= 10;
            fieldWidth += (*formatPtr) - '0';
          } else if ((*formatPtr) == '.') {
            state = FORMAT_STATE_PRECISION;
            formatPtr++;
          } else {
            state = FORMAT_STATE_LENGTH;
          }
        }

        if (state == FORMAT_STATE_PRECISION) {

        }

      }
    }
  }
}
