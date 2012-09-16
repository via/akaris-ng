SRCROOT=../..
ARCH=i686
CC=${ARCH}-elf-gcc
LD=${ARCH}-elf-ld
AS=${ARCH}-elf-as
CFLAGS+=-ffreestanding -nostdlib -fno-builtin
SRCS+= ${SRCROOT}/i686/bootvideo.c \
			 ${SRCROOT}/i686/i686.c \
			 ${SRCROOT}/i686/i686_cpu.c \
			 ${SRCROOT}/i686/i686_virtmem.c \
			 ${SRCROOT}/i686/i686_mutex.c \
			 ${SRCROOT}/i686/i686_physmem.c \
			 ${SRCROOT}/i686/stacksmash.c
LOADER_SRCS+= ${SRCROOT}/i686/loader/headers.c \
							${SRCROOT}/i686/loader/loader.c \
							${SRCROOT}/i686/i686.cpu

.include "${SRCROOT}/rules.mk"

loader_depend:
	mkdep ${CFLAG} ${SRCS}

loader:
