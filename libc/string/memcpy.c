#include <stddef.h>

void *memcpy(void *dest, const void *source, size_t num) {
    char *dst = (char *) dest;
    char *src = (char *) source;
    while(num-- != 0) {
        *dst++ = *src++;
    }
    return dest;
}
