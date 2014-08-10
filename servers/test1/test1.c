
int main() {

  int i;
  for (i = 0; i < 500000000; ++i);
  __asm__("int $0x80");
  while (1);

  return 0;
}
