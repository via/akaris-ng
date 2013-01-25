#include "types.h"
#include "multiboot.h"


multiboot_header_t header = {
  .magic = MULTIBOOT_HEADER_MAGIC,
  .flags = MULTIBOOT_HEADER_FLAGS,
  .checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS),
}; 




