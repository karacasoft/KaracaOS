file kernel/karacaos.kernel
target remote :1234
layout src

add-symbol-file example/example.elf 0x10008090

# b kmain
# b buddy_allocator.c:154
# b mm.c:184 if vaddr=0x10000000
# b __buddy_allocator__find_space if node=0xc8000450
# b test.c:146
# b _isr128
# b __arch__syscall_initial_handler
# b interrupts_init
# b arch_set_syscall_handler
# b test.c:55
# b fscontext__init
# b buddy_allocator__alloc
# b kaos_fopen
# b test.c:60
# b fat16_read_cluster_fat_chain
# b kaos_fopen
# b fat16_read_file
# b kaos_fwrite
# b syscall__handle_fwrite
# b syscall__handle_fopen
