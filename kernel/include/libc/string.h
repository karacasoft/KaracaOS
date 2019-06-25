#ifndef __STRING_H__
#define __STRING_H__ 1

#include <stddef.h>

void *kaos_memcpy(void *dest, const void *source, size_t num);
void *kaos_memset(void *str, int c, size_t n);

#endif // __STRING_H__