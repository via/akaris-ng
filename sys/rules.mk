
SUBDIRS+=${SRCROOT}/common ${SRCROOT}/lib
CFLAGS+=-Wall -Wextra -I${SRCROOT}/include -I${SRCROOT}/${ARCH} -std=c99

.for dir in ${SUBDIRS}
.include "${dir}/Makefile"
.endfor

OBJS = ${SRCS:S/.c/.o/:S/.s/.o/:T}
.if ${LOADER}
LOADER_OBJS = ${LOADER_SRCS:S/.c/.o/:S/.s/.o/:T}
.endif

kernel: kernel.k ${LOADER}

depend:
	mkdep ${CFLAGS} ${SRCS} ${LOADER_SRCS}


.if exists(${SRCROOT}/scripts/${ARCH}.ld)
kernel.k: ${OBJS}
	${LD} -o kernel.k -T ${SRCROOT}/scripts/${ARCH}.ld ${OBJS}
.endif

clean:
	-rm kernel.k ${OBJS} .depend ${LOADER_OBJS} loader.elf

loader.elf: ${LOADER_OBJS}
	${LD} -o loader.elf -T ${SRCROOT}/scripts/loader-${ARCH}.ld ${LOADER_OBJS} 

install: kernel.k ${LOADER}
	cp kernel.k ${SRCROOT}
	-cp ${LOADER} ${SRCROOT}
