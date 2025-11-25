#include <fs/fat16/fat16.h>
#include <fs/fscontext.h>

extern fat16_dir_entry_t __open_files[MAX_OPEN_FILES];
extern uint8_t __is_open_file_entry_used[MAX_OPEN_FILES];

size_t kaos_fread(void *buffer, size_t size, size_t count, FILE *stream) {
  int fileNo = stream->_fileno;
  size_t out_size = 0;
  if (fileNo == 0) {
    // TODO
  } else if (fileNo == 1) {
    // TODO
  } else if (fileNo == 2) {
    // TODO
  } else {
    fat16_dir_entry_t openFile = __open_files[fileNo];
    fat16_context_t context;
    fat16_read_file(&context, &openFile, buffer, size, &out_size);
  }
  return out_size;
}
