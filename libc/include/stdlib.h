#ifndef __STDLIB_H__
#define __STDLIB_H__ 1

#include <stddef.h>


void abort(void);
void free(void *ptr);
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);

int atexit(typeof(void (void)) *function);
int atoi(const char *nptr);

void exit(int code);

int abs(int j);

char *getenv(const char *name);

#endif
