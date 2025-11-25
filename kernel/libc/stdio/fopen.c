#include <kernel/sysdefs.h>
#include <mm/buddy_allocator.h>
#include <libc/stdio.h>
#include <libc/string.h> 
#include <fs/fat16/fat16.h>
#include <fs/fscontext.h>


fat16_dir_entry_t __open_files[MAX_OPEN_FILES] = {0};
uint8_t __is_open_file_entry_used[MAX_OPEN_FILES] = {1, 1, 1, 0};

FILE *kaos_fopen(const char *path, const char *mode) {
  SYS_RET ret;

  FILE *fp;
  ret = buddy_allocator__alloc(sizeof(FILE), (void **)&fp);
  if (ret) {
    kaos_printf("Cannot allocate memory for FILE *\n");
    return NULL;
  }
  if (kaos_strncmp("/dev/stdin", path, sizeof("/dev/stdin"))) {
    fp->_fileno = 0;
  } else if (kaos_strncmp("/dev/stdout", path, sizeof("/dev/stdout"))) {
    fp->_fileno = 1;
  } else if (kaos_strncmp("/dev/stderr", path, sizeof("/dev/stderr"))) {
    fp->_fileno = 2;
  } else {
    fat16_dir_entry_t dir_entry;

    ret = fat16_find_file_entry(&__fscontext, "/EXAMPLE.ELF", &dir_entry);
    if (ret != SYS_RET_NO_ERROR) {
      kaos_printf("Cannot find file entry\n");
      return NULL;
    }

    int i;
    for (i = 0; i < MAX_OPEN_FILES; i++) {
      if (__is_open_file_entry_used[i] == FALSE) {
        break;
      }
    }

    if (i == MAX_OPEN_FILES) {
      kaos_printf("Max open files reached\n");
      return NULL;
    }

    __open_files[i] = dir_entry;

    fp->_fileno = i;
  }

  return fp;
}
