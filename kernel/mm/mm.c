#include <mm/mm.h>
#include <libc/string.h>

extern void heap_start;
void *heap_ptr = &heap_start;

void *kaos_kernmalloc(size_t size) {
    void *curr = heap_ptr;
    kaos_memset(curr, 0, size);
    heap_ptr += size;
    return curr;
}