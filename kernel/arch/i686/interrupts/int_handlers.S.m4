define(`INT_HANDLER', `.section .text
.global _isr`'$1
.type _isr`'$1, @function
_isr`'$1:
  cli
  ifelse(`$2', `0', `pushl `$'0')

  pushl `$'$1
  jmp _isr_common_handler
.size _isr`'$1, . - _isr`'$1
'
)dnl

INT_HANDLER(0, 0)
INT_HANDLER(1, 0)
INT_HANDLER(2, 0)
INT_HANDLER(3, 0)
INT_HANDLER(4, 0)
INT_HANDLER(5, 0)
INT_HANDLER(6, 0)
INT_HANDLER(7, 0)
INT_HANDLER(8, 1)
INT_HANDLER(9, 0)
INT_HANDLER(10, 1)
INT_HANDLER(11, 1)
INT_HANDLER(12, 1)
INT_HANDLER(13, 1)
INT_HANDLER(14, 1)
INT_HANDLER(15, 0)
INT_HANDLER(16, 0)
INT_HANDLER(17, 1)
INT_HANDLER(18, 0)
INT_HANDLER(19, 0)
INT_HANDLER(20, 0)
INT_HANDLER(21, 0)
INT_HANDLER(22, 0)
INT_HANDLER(23, 0)
INT_HANDLER(24, 0)
INT_HANDLER(25, 0)
INT_HANDLER(26, 0)
INT_HANDLER(27, 0)
INT_HANDLER(28, 0)
INT_HANDLER(29, 0)
INT_HANDLER(30, 1)
INT_HANDLER(31, 0)
INT_HANDLER(32, 0)
INT_HANDLER(33, 0)
INT_HANDLER(34, 0)
INT_HANDLER(35, 0)
INT_HANDLER(36, 0)
INT_HANDLER(37, 0)
INT_HANDLER(38, 0)
INT_HANDLER(39, 0)
INT_HANDLER(40, 0)
INT_HANDLER(41, 0)
INT_HANDLER(42, 0)
INT_HANDLER(43, 0)
INT_HANDLER(44, 0)
INT_HANDLER(45, 0)
INT_HANDLER(46, 0)
INT_HANDLER(47, 0)

_isr_common_handler:
  pusha
  pushw %ds
  pushw %es
  pushw %fs
  pushw %gs

  movw `$'0x18, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs
  movl %esp, %eax
  pushl %eax
  movl $_arch_interrupt_handler, %eax
  call *%eax
  popl %eax

  popw %gs
  popw %fs
  popw %es
  popw %ds
  popa

  addl `$'8, %esp
  iret
