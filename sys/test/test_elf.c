#include "check.h"
#include "elf.h"

START_TEST (check_elf_init) {
  struct elf_context ctx;
  char header[24] = {0x7f, 'E', 'L', 'F','\0'};
  fail_unless(elf_init(&ctx, header) == ELF_SUCCESS);

  char header2[24] = {0x7f, 'X', 'X', 'X','\0'};
  fail_unless(elf_init(&ctx, header2) == ELF_NOTVALID);
} END_TEST

void check_elf_setup() {

}

void check_initialize_elf_tests(TCase *t) {
  tcase_add_checked_fixture(t, check_elf_setup, NULL);
  tcase_add_test(t, check_elf_init);
}
