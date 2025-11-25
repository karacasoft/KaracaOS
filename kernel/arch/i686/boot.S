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

.section .prepaging.text
.global _start
.type _start, @function
_start:
    cli
    movl $0xA00000, %esp
    push %eax
    push %ebx

    call arch_paging_init_lower

    mov $page_directory_start, %eax
    mov %eax, %cr3
    mov %cr0, %eax
    or $0x80000001, %eax
    mov %eax, %cr0

    xor %ebp, %ebp
    pop %ebx
    pop %eax
    push %ebp
    movl $stack_top, %esp

    # leave control to kernel
    push %eax
    push %ebx
    call kmain

    # halt the machine if the control is ever
    # left from kernel
    cli
1:  hlt
    jmp 1b
.size _start, . - _start

.section .heap
.align 32
heap_top:
    .skip 10 * 1024 * 1024
heap_bottom:
