SRCROOT=../..
ARCH=test
DEFS=-DHOSTED
SRCS+= ${SRCROOT}/test/test_slab.c \
			 ${SRCROOT}/test/test_start.c \
			 ${SRCROOT}/test/test_vmem.c \
			 ${SRCROOT}/test/test_physmem.c 
CFLAGS!=pkg-config --cflags check
LDFLAGS!=pkg-config --libs check

CFLAGS+= ${DEFS}

.include "${SRCROOT}/rules.mk"

kernel: ${OBJS}
	${CC} -o kernel ${OBJS} ${CFLAGS} ${LDFLAGS} 

