#include "check.h"
#include "elf.h"

START_TEST (check_elf_init) {
  struct elf_context ctx;
  Elf32_Ehdr header = (Elf32_Ehdr){
    .e_type = ET_EXEC,
  };
  memcpy(header.e_ident, "\177ELF", 5);

  fail_unless(elf_init(&ctx, &header) == ELF_SUCCESS);

  Elf32_Ehdr header2 = (Elf32_Ehdr){
    .e_type = ET_EXEC,
  };
  memcpy(header2.e_ident, "XXX", 4);
  fail_unless(elf_init(&ctx, &header2) == ELF_NOTELF);

  Elf32_Ehdr header3 = (Elf32_Ehdr){
    .e_type = ET_REL,
  };
  memcpy(header3.e_ident, "\177ELF", 5);
  fail_unless(elf_init(&ctx, &header3) == ELF_UNSUPPORTED);
} END_TEST

void check_elf_setup() {

}

void check_initialize_elf_tests(TCase *t) {
  tcase_add_checked_fixture(t, check_elf_setup, NULL);
  tcase_add_test(t, check_elf_init);
}
