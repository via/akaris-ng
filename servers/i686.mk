ARCH=i686
CC=${ARCH}-elf-gcc
LD=${ARCH}-elf-ld
AS=${ARCH}-elf-as
SUBDIRS+=test1

.include "rules.mk"
