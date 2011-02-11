
#include "types.h"
#include "multiboot.h"

struct multiboot_header header = {
  MULTIBOOT_HEADER_MAGIC,
  MULTIBOOT_HEADER_FLAGS,
  -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS),
};__attribute__((aligned(4)))

void
start() {
  char *vmem = (char *)0xB8000;
  *vmem = '@';
  while (1);
}
