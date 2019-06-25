#include <libc/string.h>

void *kaos_memcpy(void *dest, const void *source, size_t num) {
    char *dst = (char *) dest;
    char *src = (char *) source;
    while(num-- != 0) {
        *dst++ = *src++;
    }
    return dest;
}