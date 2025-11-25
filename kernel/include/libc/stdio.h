#ifndef __STDIO_H__
#define __STDIO_H__ 1

#include <stddef.h>
#include <stdarg.h>

#define MAX_OPEN_FILES 64

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

size_t kaos_printf(const char *format, ...);
FILE *kaos_fopen(const char *path, const char *mode);
size_t kaos_fread(void *buffer, size_t size, size_t count, FILE *stream);
size_t kaos_fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream);

#endif
