
SUBDIRS+=${SRCROOT}/common ${SRCROOT}/lib
CFLAGS+=-Wall -Wextra -I${SRCROOT}/include -I${SRCROOT}/${ARCH} -std=c99

.for dir in ${SUBDIRS}
.include "${dir}/Makefile"
.endfor

OBJS = ${SRCS:S/.c/.o/:S/.s/.o/:T}

kernel: kernel.k ${LOADER}

depend:
	mkdep ${CFLAGS} ${SRCS} ${LOADER_SRCS}


.if exists(${SRCROOT}/scripts/${ARCH}.ld)
kernel.k: ${OBJS}
	${LD} -o kernel.k -T ${SRCROOT}/scripts/${ARCH}.ld ${OBJS}
.endif

clean:
	-rm kernel.k ${OBJS} .depend 

install: kernel.k 
	cp kernel.k ${SRCROOT}
