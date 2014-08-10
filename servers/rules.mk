
SRCROOT=$(pwd)
CFLAGS+=-Wall -Wextra -I${SRCROOT}/include -std=c99 -ggdb -O0
LDFLAGS+=-T link.ld

.for dir in ${SUBDIRS}
.include "${dir}/Makefile"
.endfor

OBJS = ${SRCS:S/.c/.o/:S/.s/.o/:T}

depend:
	mkdep ${CFLAGS} ${SRCS} ${LOADER_SRCS}


