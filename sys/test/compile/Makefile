SRCROOT=../..
ARCH=test
SRCS+= ${SRCROOT}/test/test_slab.c \
			 ${SRCROOT}/test/test_start.c \
			 ${SRCROOT}/test/test_vmem.c \
			 ${SRCROOT}/test/test_physmem.c 
CFLAGS!=pkg-config --cflags check
LDFLAGS!=pkg-config --libs check

.include "${SRCROOT}/rules.mk"

kernel: ${OBJS}
	${CC} -o kernel ${OBJS} ${CFLAGS} ${LDFLAGS}

