#ifndef BUDDY_H
#define BUDDY_H

struct buddy_allocator {
  int granularity;
  int levels;
  int page_size;
  void *map;
};



#endif

