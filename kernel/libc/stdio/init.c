#include <libc/stdio.h>

void __kaos_init_stdlib() {
  kaos_stdin = kaos_fopen("/dev/stdin", "r");
  kaos_stdout = kaos_fopen("/dev/stdout", "w");
  kaos_stderr = kaos_fopen("/dev/stderr", "w");
}
