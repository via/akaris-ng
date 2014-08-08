#include "elf.h"

elf_err_t elf_init(struct elf_context *ctx, const void *image) {
  ctx->hdr = image;
  if (!IS_ELF(*ctx->hdr)) {
    return ELF_NOTVALID;
  }
  return ELF_SUCCESS;
}

