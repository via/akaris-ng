
#include "sysconf.h"
#include "types.h"
#include "strfuncs.h"

#define COLS 80
#define ROWS 25

static uint16 curX = 0;
static uint16 curY = 0;
static uint16 *vmem = (uint16 *)0xB8000;

static void bootvideo_scroll() {
  if (curX >= COLS) {
    curX = 0;
    curY++;
  }
  if (curY >= ROWS) {
    int curpos;
    for (curpos = 0; curpos < (ROWS - 1) * COLS; ++curpos) {
      vmem[curpos] = vmem[curpos + COLS];
    }
    memset(vmem + COLS * (ROWS - 1), 0, COLS);
    curY = ROWS - 1;
  }
}



static void
bootvideo_putc(const char c) {
  vmem[(curY * COLS) + curX] = (uint16)c + (7 << 8);
  curX++;
  bootvideo_scroll();
}

void bootvideo_cls() {
  memset(vmem, 0, ROWS * COLS * 2);
  curX = curY = 0;
}


size_t bootvideo_puts(const char *s) {

  const char *str = s;
  char c;

  while ( (c = *s++) ) {
    bootvideo_putc(c);
  }

  return s - str - 1;
}
