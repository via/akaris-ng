ENTRY (start)

SECTIONS{
    . = 0x00100000;

    .text :{
        headers.o
        loader.o(.text)
    }

    .rodata ALIGN (0x1000) : {
        loader.o(.rodata)
    }

    .data ALIGN (0x1000) : {
        loader.o(.data)
    }

    .bss : {
        sbss = .;
        loader.o(COMMON)
        loader.o(.bss)
        ebss = .;
        start_free = ALIGN(. + SIZEOF(.test));
    }

    . = 0xC0000000 + ABSOLUTE(ebss);
    .test : AT(ABSOLUTE(ebss)) {
        test.o
    }

}
