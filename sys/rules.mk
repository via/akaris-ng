
SUBDIRS+=${SRCROOT}/common ${SRCROOT}/lib
CFLAGS+=-Wall -Wextra -I${SRCROOT}/include  -std=c99

.for dir in ${SUBDIRS}
.include "${dir}/Makefile"
.endfor

OBJS = ${SRCS:.c=.o:.s=.o:T}

depend:
	mkdep ${CFLAGS} ${SRCS}


.if exists(${SRCROOT}/scripts/${ARCH}.ld)
kernel.k: ${OBJS}
	${LD} -o kernel.k -T ${SRCROOT}/scripts/${ARCH}.ld ${OBJS}
.endif


loader:

clean:
	-rm kernel.k ${OBJS}

