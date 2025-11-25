file kernel/karacaos.kernel
target remote :1234
layout src

# b buddy_allocator.c:154
# b mm.c:184 if vaddr=0x10000000
# b __buddy_allocator__find_space if node=0xc8000450
# b test.c:146
# b _isr128
# b __arch__syscall_initial_handler
# b interrupts_init
b arch_set_syscall_handler

