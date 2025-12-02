#include <stdio.h>

void kaos_libc_init() {
  // TODO GDT and stuff
  stdin = fopen("/dev/stdin", "r");
  stdout = fopen("/dev/stdout", "w");
  stderr = fopen("/dev/stderr", "w");
}

void _init() {

}
