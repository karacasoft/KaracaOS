#include "libc/string.h"
#include <fs/fat16/fat16.h>
#include <fs/fscontext.h>

extern fat16_dir_entry_t __open_files[MAX_OPEN_FILES];
extern uint8_t __is_open_file_entry_used[MAX_OPEN_FILES];

int kaos_fclose(FILE *stream) {
  int fileNo = stream->_fileno;
  __is_open_file_entry_used[fileNo] = 0;

  kaos_memset((void *) &__open_files[fileNo], 0, sizeof(fat16_dir_entry_t));

  return 0;
}


