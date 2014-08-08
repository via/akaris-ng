#ifndef _COMMON_ELF_H
#define _COMMON_ELF_H

#include "abi_elf.h"

typedef enum {
  ELF_SUCCESS,
  ELF_NOTVALID,
} elf_err_t;

struct elf_context {
  const Elf32_Ehdr *hdr;
};

elf_err_t elf_init(struct elf_context *, const void *image);

#endif

