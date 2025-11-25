/* x86 crtn.S */
.section .init
    popl %ebp
    ret

.section .fini
    popl %ebp
    ret