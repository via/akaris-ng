#include "elf.h"

elf_error_t 
elf_init(struct elf_context *ctx, const void *image) {
  ctx->hdr = image;
  ctx->cur_pg = 0;
  if (!IS_ELF(*ctx->hdr)) {
    return ELF_NOTELF;
  }
  if (ctx->hdr->e_type != ET_EXEC) {
    return ELF_UNSUPPORTED;
  }
  return ELF_SUCCESS;
}

elf_error_t
elf_iterate(struct elf_context *ctx) {
  const Elf32_Phdr *pg;
  if (ctx->cur_pg >= ctx->hdr->e_phnum) {
    return ELF_END;
  }
  pg = (const Elf32_Phdr *)((const char *)ctx->hdr + ctx->hdr->e_phoff + 
      (ctx->hdr->e_phentsize * ctx->cur_pg));
  if (pg->p_type != PT_LOAD) {
    return ELF_UNSUPPORTED;
  }
  ctx->section.elf_start = (const void *)(pg->p_offset + (void *)ctx->hdr);
  ctx->section.elf_len = pg->p_memsz;
  ctx->section.memaddr = (void *)pg->p_vaddr;
  ctx->section.executable = (pg->p_flags & PF_X) != 0;
  ctx->section.writable = (pg->p_flags & PF_W) != 0;
  ctx->section.readable = (pg->p_flags & PF_R) != 0;
  ctx->cur_pg++;
  return ELF_SUCCESS;
}

