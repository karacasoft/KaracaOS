#ifndef __STRING_H__
#define __STRING_H__ 1

typedef long unsigned int size_t;

void *memcpy(void *dest, const void *source, size_t num);
void *memset(void *str, int c, size_t n);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t n);

char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t n);
char *strcat(char *dst, const char *src);

char *strchr(const char *s, int c);

#endif
