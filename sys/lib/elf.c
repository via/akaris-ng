#include "abi_elf.h"
#include "elf.h"

int elf_valid_header(const void *image) {
  Elf32_Ehdr *hdr = (Elf32_Ehdr *)image;
  return IS_ELF(*hdr);
}
