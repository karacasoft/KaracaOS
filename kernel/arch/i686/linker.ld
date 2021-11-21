ENTRY(_start)

SECTIONS
{
    . = 1M;
    .prepaging.text : ALIGN(4K)
    {
        *(.multiboot)
        *(.prepaging.text)
    }

    .prepaging.data : ALIGN(4K)
    {
        *(.prepaging.data)
    }

    . = 5M;
    .pagetables : ALIGN(4K)
    {
        page_directory_start = .;
    }

    . = 10M;
    . += 0xC0000000;
    .text ALIGN(4K) : AT (ADDR (.text) - 0xC0000000)
    {
        *(.text)
    }

    .rodata ALIGN(4K) : AT (ADDR (.rodata) - 0xC0000000)
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT (ADDR (.data) - 0xC0000000)
    {
        *(.data)
    }

    .bss ALIGN(4K) : AT (ADDR (.bss) - 0xC0000000)
    {
        *(.bss)
    }
    
    .heap ALIGN(4K) : AT (ADDR (.heap) - 0xC0000000)
    {
        heap_start = .;
        *(.heap)
    }

    

}