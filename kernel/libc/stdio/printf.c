#include <libc/stdio.h>
#include <tty/tty.h>
#include <kernel/sysdefs.h>

size_t kaos_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    tty_handle_t *tty = tty_getdefaulthandle();
    size_t numCh = 0;
    while(TRUE) {
        if(*format == '%') {
            format++;
            if(*format == '%') {
                tty_putch(tty, '%');
                numCh++;
            } else if(*format == 'd') {
                char numAsStr[15] = {0};
                int num = va_arg(args, int);
                uint8_t digitCount = 0;
                char *ptr = &numAsStr[13];
                int n = (num < 0) ? -num : num;
                while(n > 0) {
                    int digit = n % 10;
                    *ptr-- = digit + '0';
                    n = n / 10;
                    digitCount++;
                }
                if(num == 0) { *ptr-- = '0'; digitCount++; }
                if(num < 0) {
                    tty_putch(tty, '-');
                    tty_puts(tty, ptr+1, digitCount);
                    numCh += digitCount + 1;
                } else {
                    tty_puts(tty, ptr+1, digitCount);
                    numCh += digitCount;
                }
            } else if(*format == 'x') {
                char numAsStr[15] = {0};
                int num = va_arg(args, int);
                uint8_t digitCount = 0;
                char *ptr = &numAsStr[13];
                int n = (num < 0) ? -num : num;
                while(n > 0) {
                    int digit = n % 0x10;
                    if(digit < 10) {
                        *ptr-- = digit + '0';
                    } else {
                        *ptr-- = (digit - 10) + 'a';
                    }
                    n = n / 0x10;
                    digitCount++;
                }
                if(num == 0) { *ptr-- = '0'; digitCount++; }
                if(num < 0) {
                    tty_putch(tty, '-');
                    tty_puts(tty, ptr+1, digitCount);
                    numCh += digitCount + 1;
                } else {
                    tty_puts(tty, ptr+1, digitCount);
                    numCh += digitCount;
                }
            } else if(*format == 's') {
                // TODO max 100 characters, change this later
                tty_puts(tty, va_arg(args, char *), 100);
            } else if(*format == 'c') {
                tty_putch(tty, (char) va_arg(args, int));
            } else if(*format == 0) {
                break;
            }
        } else if(*format == 0) {
            break;
        } else {
            tty_putch(tty, *format);
            numCh++;
        }
        format++;
    }
    return numCh;
}