#include "abi_elf.h"
#include "elf.h"

int elf_valid_header(struct elf_context *c) {
  Elf32_Ehdr *hdr = (Elf32_Ehdr *)c->image;
  return IS_ELF(*hdr);
}
