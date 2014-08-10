#ifndef _COMMON_ELF_H
#define _COMMON_ELF_H

#include "abi_elf.h"

typedef enum {
  ELF_SUCCESS,
  ELF_NOTELF,
  ELF_END,
  ELF_UNSUPPORTED,
} elf_error_t;

struct elf_section {
  const void *elf_start;
  void *memaddr;
  size_t elf_len;
  int writable : 1;
  int executable : 1;
  int readable : 1;
};

struct elf_context {
  const Elf32_Ehdr *hdr;
  int cur_pg;
  struct elf_section section;
};


elf_error_t elf_iterate(struct elf_context *);
elf_error_t elf_init(struct elf_context *, const void *image);

static inline struct elf_section *
elf_section(struct elf_context *ctx) {
  return &ctx->section;
}


#endif

