SRCROOT=../..
ARCH=test
DEFS=-DHOSTED -DUNITTEST

SRCS+= ${SRCROOT}/test/test.c \
       ${SRCROOT}/test/test_slab.c \
			 ${SRCROOT}/test/test_vmem.c \
			 ${SRCROOT}/test/test_physmem.c \
			 ${SRCROOT}/test/test_address_space.c

CFLAGS!=pkg-config --cflags check
LDFLAGS!=pkg-config --libs check

CFLAGS+= ${DEFS}

.include "${SRCROOT}/rules.mk"

kernel: ${OBJS}
	${CC} -o kernel ${OBJS} ${CFLAGS} ${LDFLAGS} 

