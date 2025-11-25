#include <stdint.h>

void *memset(void *str, int c, size_t n) {
    uint8_t *ref = (uint8_t *) str;
    while(n-- > 0) *ref++ = (uint8_t)c;
    return str;
}
