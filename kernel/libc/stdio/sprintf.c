#include <libc/stdio.h>
#include <kernel/sysdefs.h>
#include <stdint.h>
#include <stddef.h>

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

// out of standard, internal only
#define FORMAT_FLAG_UNSIGNED 0x20
#define FORMAT_FLAG_UPPERCASE_HEX 0x40
#define FORMAT_FLAG_PRECISION_DEFINED 0x80

#define FORMAT_LENGTH_CHAR 0x1
#define FORMAT_LENGTH_SHORT 0x2
#define FORMAT_LENGTH_LONG 0x4
#define FORMAT_LENGTH_LONG_LONG 0x8
#define FORMAT_LENGTH_INTMAX_T 0x10
#define FORMAT_LENGTH_SIZE_T 0x20
#define FORMAT_LENGTH_PTRDIFF_T 0x40
#define FORMAT_LENGTH_LONG_DOUBLE 0x80

#define FORMAT_INT_TO_STR_BUFFER_SIZE 255

size_t convert_any_int_to_str(long long int arg, uint32_t flags, int fieldWidth,
                              int precision, int radix, char *out_buffer);

size_t convert_any_int_to_str(long long int arg, uint32_t flags, int fieldWidth,
                              int precision, int radix, char *out_buffer) {
  uint32_t i = 0;
  uint32_t char_count = 0;
  uint32_t extra_chars = 0;
  long long int arg_temp = arg;
  if (!(flags & FORMAT_FLAG_UNSIGNED) &&
      (flags & FORMAT_FLAG_BEGIN_WITH_SIGN)) {
    char_count++;
    extra_chars++;
  } else if (!(flags & FORMAT_FLAG_UNSIGNED) && arg < 0) {
    char_count++;
    extra_chars++;
    arg_temp = (~arg_temp) + 1;
  }
  if (flags & FORMAT_FLAG_ALT_FORM) {
    if (radix == 16) {
      char_count += 2;
      extra_chars += 2;
    }
  }
  while (arg_temp != 0) {
    arg_temp /= radix;
    char_count++;
  }
  if (!precision)
    precision = 1;
  if (radix == 8 && char_count > precision) {
    char_count++;
  }
  if (char_count < precision)
    char_count = precision + extra_chars;

  // padding
  if (fieldWidth && char_count < fieldWidth) {
    char pad_char = (flags & FORMAT_FLAG_PAD_WITH_ZEROES) ? '0' : ' ';
    for (i = 0; i < fieldWidth; i++) {
      out_buffer[i] = pad_char;
    }
    i = 0;
  }

  arg_temp = arg;
  if (arg < 0) {
    arg_temp = (~arg_temp) + 1;
  }

  if (flags & FORMAT_FLAG_LEFT_JUSTIFY) {
    i += char_count;
  } else {
    if (fieldWidth > char_count) {
      i += fieldWidth;
    } else {
      i += char_count;
    }
  }

  char hex_base_char = (flags & FORMAT_FLAG_UPPERCASE_HEX) ? 'A' : 'a';

  uint32_t j;
  for (j = 0; j < char_count - extra_chars; j++) {
    long long int digit = arg_temp % radix;
    arg_temp /= radix;
    i--;
    if (digit >= 0 && digit <= 9) {
      out_buffer[i] = digit + '0';
    } else {
      out_buffer[i] = hex_base_char + digit - 10;
    }
  }

  if (flags & FORMAT_FLAG_ALT_FORM) {
    if (radix == 16) {
      i--;
      out_buffer[i] = hex_base_char + ('X' - 'A');
    }
  }

  if (!(flags & FORMAT_FLAG_UNSIGNED) &&
      (flags & FORMAT_FLAG_BEGIN_WITH_SIGN)) {
    i--;
    out_buffer[i] = arg < 0 ? '-' : '+';
  } else if (!(flags & FORMAT_FLAG_UNSIGNED) &&
             (flags & FORMAT_FLAG_BEGIN_WITH_SIGN)) {
    i--;
    out_buffer[i] = arg < 0 ? '-' : '+';
  } else if (!(flags & FORMAT_FLAG_UNSIGNED) && (arg < 0)) {
    i--;
    out_buffer[i] = '-';
  }

  return fieldWidth > char_count ? fieldWidth : char_count;
}
int kaos_vsprintf(char *str, const char *format, va_list args) {
  char *bufferPtr = str;

  const char *formatPtr = format;
  while (*formatPtr != '\0') {
    if (*formatPtr == '%') {
      unsigned char state = FORMAT_STATE_FLAGS;
      unsigned int flags = 0;
      int fieldWidth = 0;
      int precision = 0;
      unsigned int length = 0;
      BOOLEAN formatFinished = FALSE;
      size_t printedCharCount = 0;
      while (!formatFinished) {
        formatPtr++;
        if (state == FORMAT_STATE_FLAGS) {
          switch (*formatPtr) {
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
          } else if ((*formatPtr) == '*') {
            fieldWidth = va_arg(args, int);
          } else if ((*formatPtr) == '.') {
            state = FORMAT_STATE_PRECISION;
            formatPtr++;
          } else {
            state = FORMAT_STATE_LENGTH;
          }
        }

        if (state == FORMAT_STATE_PRECISION) {
          if ((*formatPtr) >= '0' && (*formatPtr) <= '9') {
            precision *= 10;
            precision += (*formatPtr) - '0';
            flags |= FORMAT_FLAG_PRECISION_DEFINED;
          } else if ((*formatPtr) == '*') {
            precision = va_arg(args, int);
            flags |= FORMAT_FLAG_PRECISION_DEFINED;
          } else {
            state = FORMAT_STATE_LENGTH;
          }
        }

        if (state == FORMAT_STATE_LENGTH) {
          if ((*formatPtr) == 'h' && (*(formatPtr + 1)) == 'h') {
            length |= FORMAT_LENGTH_CHAR;
            formatPtr++;
          } else if ((*formatPtr) == 'h') {
            length |= FORMAT_LENGTH_SHORT;
          } else if ((*formatPtr) == 'l') {
            length |= FORMAT_LENGTH_LONG;
          } else if ((*formatPtr) == 'l' && (*(formatPtr + 1)) == 'l') {
            length |= FORMAT_LENGTH_LONG_LONG;
            formatPtr++;
          } else if ((*formatPtr) == 'j') {
            length |= FORMAT_LENGTH_INTMAX_T;
          } else if ((*formatPtr) == 'z') {
            length |= FORMAT_LENGTH_SIZE_T;
          } else if ((*formatPtr) == 't') {
            length |= FORMAT_LENGTH_PTRDIFF_T;
          } else if ((*formatPtr) == 'L') {
            length |= FORMAT_LENGTH_LONG_DOUBLE;
          } else {
            formatPtr--;
          }
          formatPtr++;
          state = FORMAT_STATE_SPECIFIER;
        }

        if (state == FORMAT_STATE_SPECIFIER) {
          long long int arg;
          if (
              (*formatPtr) == 'd' ||
              (*formatPtr) == 'i' ||
              (*formatPtr) == 'o' ||
              (*formatPtr) == 'u' ||
              (*formatPtr) == 'x' ||
              (*formatPtr) == 'X' ||
              (*formatPtr) == 'p'
          ) {
            switch (length) {
              case FORMAT_LENGTH_LONG_DOUBLE:
                arg = va_arg(args, long long int);
                break;
              case FORMAT_LENGTH_PTRDIFF_T:
                arg = va_arg(args, ptrdiff_t);
                break;
              case FORMAT_LENGTH_INTMAX_T:
                arg = va_arg(args, intmax_t);
                break;
              case FORMAT_LENGTH_SIZE_T:
                arg = va_arg(args, size_t);
                break;
              case FORMAT_LENGTH_LONG_LONG:
                arg = va_arg(args, long long int);
                break;
              case FORMAT_LENGTH_LONG:
                arg = va_arg(args, long int);
                break;
              case FORMAT_LENGTH_CHAR:
              case FORMAT_LENGTH_SHORT:
              default:
                arg = va_arg(args, int);
                break;
            }

          }
          if ((*formatPtr) == 'd' || (*formatPtr) == 'i') {
            printedCharCount = convert_any_int_to_str(arg, flags, fieldWidth, precision, 10,
                                   bufferPtr);
            bufferPtr += printedCharCount;
            formatPtr++;
            formatFinished = TRUE;
          } else if ((*formatPtr) == 'o') {
            flags |= FORMAT_FLAG_UNSIGNED;
            printedCharCount = convert_any_int_to_str(arg, flags, fieldWidth, precision, 8,
                                   bufferPtr);
            bufferPtr += printedCharCount;
            formatPtr++;
            formatFinished = TRUE;
          } else if ((*formatPtr) == 'u') {
            flags |= FORMAT_FLAG_UNSIGNED;
            printedCharCount = convert_any_int_to_str(arg, flags, fieldWidth, precision, 8,
                                   bufferPtr);
            bufferPtr += printedCharCount;
            formatPtr++;
            formatFinished = TRUE;
          } else if (((*formatPtr) == 'x') || ((*formatPtr) == 'X') || ((*formatPtr) == 'p')) {
            flags |= FORMAT_FLAG_UNSIGNED;
            if (*formatPtr == 'X') flags |= FORMAT_FLAG_UPPERCASE_HEX;
            printedCharCount = convert_any_int_to_str(arg, flags, fieldWidth, precision, 16,
                                   bufferPtr);
            bufferPtr += printedCharCount;
            formatPtr++;
            formatFinished = TRUE;
          } else if ((*formatPtr) == 'c') {
            // TODO: no wchar_t support
            unsigned char arg = va_arg(args, int);
            *bufferPtr++ = arg;
            formatPtr++;
            printedCharCount = 1;
            formatFinished = TRUE;
          } else if ((*formatPtr) == 's') {
            // TODO: no wchar_t support
            char *arg = va_arg(args, char *);
            while(*arg != '\0') {
              if (flags & FORMAT_FLAG_PRECISION_DEFINED) {
                if (precision <= printedCharCount) {
                  break;
                }
              }
              printedCharCount++;
              *bufferPtr++ = *arg;
              arg++;
            }
            formatPtr++;
            formatFinished = TRUE;
          } else if ((*formatPtr) == '%') {
            *bufferPtr++ = '%';
            formatFinished = TRUE;
          } else if ((*formatPtr) == 'n') {
            int *charcount = va_arg(args, int *);
            *charcount = bufferPtr - str;
            formatPtr++;
            formatFinished = TRUE;
          }
        }
      }

    } else {
      *bufferPtr++ = *formatPtr++;
    }
  }
  *bufferPtr = '\0';
  return bufferPtr - str;
}


int kaos_sprintf(char *str, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int retVal = kaos_vsprintf(str, format, args);
  va_end(args);
  return retVal;
}
