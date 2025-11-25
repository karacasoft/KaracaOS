#include "tty/tty.h"
#include <libc/stdio.h>

size_t kaos_fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream) {
  int fileNo = stream->_fileno;
  if (size != 1) {
    return 0;
  }

  if (fileNo == 0) {
    return 0;
  }

  if (fileNo == 1 || fileNo == 2) {
    tty_handle_t *tty = tty_getdefaulthandle();
    tty_puts(tty, ptr, nitems);
    return nitems;
  }

  // TODO
  return 0;
}
