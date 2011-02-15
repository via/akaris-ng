#include "types.h"
#include "multiboot.h"


struct multiboot_header header = {
  MULTIBOOT_HEADER_MAGIC,
  MULTIBOOT_HEADER_FLAGS,
  -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS),
}; 




