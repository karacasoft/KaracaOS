.section .text

.global _start

_start:
  mov $0, %ebp
  push %ebp
  push %ebp
  mov %esp, %ebp

  push %esi
  push %edi

  call kaos_libc_init

  call _init

  pop %edi
  pop %esi

  call main

  mov %eax, %edi
  call exit

.size _start, . - _start


