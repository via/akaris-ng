SRCROOT=../..
ARCH=i686
CC=${ARCH}-elf-gcc
LD=${ARCH}-elf-ld
AS=${ARCH}-elf-as
LOADER=loader.elf
CFLAGS+=-ffreestanding -nostdlib -fno-builtin -fstack-protector
SRCS+= ${SRCROOT}/i686/bootvideo.c \
			 ${SRCROOT}/i686/i686.c \
			 ${SRCROOT}/i686/i686_cpu.c \
			 ${SRCROOT}/i686/i686_virtmem.c \
			 ${SRCROOT}/i686/i686_mutex.c \
			 ${SRCROOT}/i686/i686_physmem.c \
			 ${SRCROOT}/i686/stacksmash.c
LOADER_SRCS+= ${SRCROOT}/i686/loader/headers.c \
							${SRCROOT}/i686/loader/loader.c \


.include "${SRCROOT}/rules.mk"
