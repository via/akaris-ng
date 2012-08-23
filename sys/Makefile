
.include "rules.def.mk"
.if exists("rules.mk")
.include "rules.mk"
.endif

SUBDIR=common lib
CFLAGS=-Wall -Wextra -ffreestanding -nostdlib -fno-builtin -I./include -I./lib -std=c99
CC=${ARCH}-elf-gcc
LD=${ARCH}-elf-ld
AS=${ARCH}-elf-as

SUBDIR+=${ARCH}
CFLAGS+=-I${ARCH}

.for dir in ${SUBDIR}
.include "${dir}/Makefile"
.endfor

OBJS = ${SRCS:.c=.o:.s=.o}

depend:
	mkdep ${CFLAGS} ${SRCS}


kernel.k: ${OBJS}
	${LD} -o kernel.k -T scripts/${ARCH}.ld ${OBJS}

loader:

clean:
	-rm kernel.k ${OBJS}

#.c.o:
#	${CC} -o $@ -Wa,-a,-ad  ${CFLAGS} $< > ${<}.lst
