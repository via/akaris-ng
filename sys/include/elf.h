#ifndef _COMMON_ELF_H
#define _COMMON_ELF_H

enum elf_error_t {
  ELF_SUCCESS,
  ELF_NOTELF,
  ELF_END,
  ELF_UNSUPPORTED,
};


struct elf_section {
  virtaddr elf_start;
  size_t elf_len;
  int writable : 1;
  int executable : 1;
  int readable : 1;
};

struct elf_context {
  const void *image;
  struct elf_section section;
};


void elf_open(struct elf_context *, const void *image);
int elf_valid_header(struct elf_context *);
elf_error_t elf_load(struct elf_context *);

#define ELF_SECTION(x) ((x).section)

#endif

