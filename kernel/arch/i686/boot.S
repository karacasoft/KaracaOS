.set MAGIC, 0x1BADB002
.set ALIGNED, 1<<0
.set MEMINFO, 1<<1
.set OS_FLAGS, ALIGNED | MEMINFO
.set CHECKSUM, -(MAGIC + OS_FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long OS_FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
    .skip 65536
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    movl $stack_top, %esp
    # global constructors
    call _init

    xor %ebp, %ebp
    push %ebp
    movl $stack_top, %esp

    # leave control to kernel
    call kmain

    # halt the machine if the control is ever
    # left from kernel
    cli
1:  hlt
    jmp 1b
.size _start, . - _start