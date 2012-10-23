ENTRY (loader_start)

SECTIONS{
    . = 0x00100000;

    .loader :{
      headers.o
      loader.o
    }

    highstart = 0xC0000000;
    . = highstart + .;
    .text ALIGN(0x1000) : AT(ADDR(.text) - highstart) {
        *(.text)
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - highstart) {
        *(.rodata)
    }

    .data ALIGN (0x1000) : AT(ADDR(.data) - highstart) {
        *(.data)
    }

    .bss : AT(ADDR(.bss) - highstart) {
        sbss = .;
        *(COMMON)
        *(.bss)
        phys_start_free = . - highstart;
    }

}
