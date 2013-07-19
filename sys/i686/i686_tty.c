#include "i686_io.h"
#include "i686_tty.h"

static const int tty_addrs[] = {0x3F8};

void i686_tty_init(int com, int baud) {
  int tty_addr = tty_addrs[com];
  i686_outb(tty_addr + 1, 0x00);
  i686_outb(tty_addr + 3, 0x80);
  i686_outb(tty_addr + 0, 115200 / baud); /* 9600 */
  i686_outb(tty_addr + 1, 0x00);
  i686_outb(tty_addr + 3, 0x03);
  i686_outb(tty_addr + 2, 0xC7);
  i686_outb(tty_addr + 4, 0x0B);
}

static int i686_tty_fifo_full(int com) {
  return (i686_inb(tty_addrs[com] + 5) & 0x20) == 0;
}

void i686_tty_putchar(int com, char c) {
  while (i686_tty_fifo_full(com));

  i686_outb(tty_addrs[com], c);
}

void i686_tty_puts(int com, char *s) {
  char c;
  while ( (c = *s++) ) {
    i686_tty_putchar(com, c);
  }

}




