#include "elf.h"

elf_error_t elf_init(struct elf_context *ctx, const void *image) {
  ctx->hdr = image;
  if (!IS_ELF(*ctx->hdr)) {
    return ELF_NOTELF;
  }
  return ELF_SUCCESS;
}

