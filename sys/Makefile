CFLAGS=-g -O0 -Wall -Werror -Wextra -ffreestanding -nostdlib -fno-builtin -I ./include -std=c89
i386_objs +=i386/physmem/physmem.o

kernel_objs += ${i386_objs}

all: ${kernel_objs}
	ld -Tlinkerscript.ld -o kernel.k ${kernel_objs}

.c.o:
	${CC} -o $@ -Wa,-a,-ad  ${CFLAGS} $< > ${<}.lst
