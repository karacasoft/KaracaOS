#include <mm.h>
#include <libc/string.h>

extern void heap_start;
void *heap_ptr = &heap_start;

void *kaos_kernmalloc(size_t size) {
    void *curr = heap_ptr;
    kaos_memset(curr, 0, size);
    heap_ptr += size;
    return curr;
}

SYS_RET mm_init() {
    uint32_t i;
    for(i = 0; i < 512; i++) {
        mm_page_alloc_bitmap[i] = 0xFF;
    }

    for(i = 98304; i < 131072; i++) {
        mm_vm_alloc_bitmap[i] = 0xFF;
    }
}

uint8_t mm_is_memory_available(uint32_t size) {
    // TODO check available memory return 0 or 1
}

SYS_RET mm_alloc(void **ret, uint32_t vaddr, uint32_t size, uint32_t flags) {
    if(VM_ISALLOCATED_PAGE(vaddr / 4096)) {
        return SYS_RET_MEM_ALLOC_ERROR;
    }



    // TODO keep allocated memory somewhere
}

SYS_RET mm_unalloc(void *ret) {

}