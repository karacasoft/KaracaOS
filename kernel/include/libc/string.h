#ifndef __STRING_H__
#define __STRING_H__ 1

#include <stddef.h>

void *kaos_memcpy(void *dest, const void *source, size_t num);
void *kaos_memset(void *str, int c, size_t n);
char *kaos_strcpy(char *dest, const char *source);
char *kaos_strncpy(char *dest, const char *source, size_t n);
size_t kaos_strlen(const char *s);
size_t kaos_strnlen(const char *s, size_t n);
int kaos_strcmp(const char *s1, const char *s2);
int kaos_strncmp(const char *s1, const char *s2, size_t n);

#endif // __STRING_H__
