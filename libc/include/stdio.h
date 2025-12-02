#ifndef __STDIO_H__
#define __STDIO_H__ 1

#include <stddef.h>
#include <stdarg.h>

struct __IO_FILE {
  int _flags;

  char *_IO_read_ptr;
  char *_IO_read_end;
  char *_IO_read_base;
  char *_IO_write_base;
  char *_IO_write_ptr;
  char *_IO_write_end;
  char *_IO_buf_base;
  char *_IO_buf_end;

  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  int _fileno;
  int _flags2;
};

typedef struct __IO_FILE FILE;

extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

#define SEEK_SET 0

int vfprintf(FILE *stream, const char *format, va_list args);
int printf(const char *format, ...);
int vsprintf(char *str, const char *format, va_list args);
int sprintf(char *str, const char *format, ...);

int fflush(FILE *stream);
int fprintf(FILE *stream, const char * format, ...);
int fclose(FILE *stream);
FILE *fopen(const char *path, const char *mode);
size_t fread(void *buffer, size_t size, size_t count, FILE *stream);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t n, FILE *stream);
int feof (FILE *stream);

void setbuf(FILE *stream, char *buf, size_t size);

#endif
