#ifndef SYS_I686_IO_H
#define SYS_I686_IO_H

static inline void 
i686_outb(unsigned short addr, unsigned char byte) {
  __asm__ volatile( "outb %0, %1" : : "a"(byte), "Nd"(addr));
}
  
static inline char 
i686_inb(unsigned short addr) {
  unsigned char byte;
  __asm__ volatile( "inb %1, %0" : "=a"(byte) : "Nd"(addr));
  return byte;
}

#endif

