#include "check.h"
#include "elf.h"

START_TEST (check_elf_valid_header) {
  char header[24] = {0x7f, 'E', 'L', 'F','\0'};
  fail_unless(elf_valid_header(header));

  char header2[24] = {0x7f, 'X', 'X', 'X','\0'};
  fail_if(elf_valid_header(header2));
} END_TEST

void check_elf_setup() {

}

void check_initialize_elf_tests(TCase *t) {
  tcase_add_checked_fixture(t, check_elf_setup, NULL);
  tcase_add_test(t, check_elf_valid_header);
}
