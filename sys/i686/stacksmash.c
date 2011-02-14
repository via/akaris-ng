void * __stack_chk_guard = 0;

void __stack_chk_guard_setup()
{
  unsigned char * p;
  p = (unsigned char *) &__stack_chk_guard;
  /* 32bit code, obviously */
  p[0] = 0;
  p[1] = 0;
  p[2] = '\n';
  p[3] = 133; /* <- this should probably be randomized */
}

void __attribute__((noreturn)) __stack_chk_fail()
{ 
  /* put your panic function or similar in here */
  unsigned char * vid = (unsigned char *)0xB8000;
  vid[1] = 7;
  for(;;)
    vid[0]++;
}
