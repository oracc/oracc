#include <stdio.h>
#include <types.h>
#include <se.h>

int
main(int argc, char **argv)
{
  unsigned long v;
  unsigned long lmask = 
    0x01000000
    +0x02000000
    +0x04000000
    +0x08000000
    +0x10000000
    +0x20000000
    +0x40000000;

  printf("mask=%x\n", (unsigned)lmask);

  v = 123456;
  printf("123456: %lu\n", v);
  if (!isQ(v))
    printf("P123456: P%lu\n", v);
  setQ(v);
  printf("Q123456: %c%lu\n",isQ(v) ? 'Q' : 'P', (long)idVal(v));

  v |= lm_sux;
  printf("Q123456/sux: %c%lu lang=%s\n",isQ(v) ? 'Q' : 'P', (long)idVal(v),
	 lmVal(v)==lm_sux ? "sux" : "unknown");

  v = 123456;
  v |= lm_sux;
  printf("P123456/sux: %c%lu lang=%s\n",isQ(v) ? 'Q' : 'P', (long)idVal(v),
	 lmVal(v)==lm_sux ? "sux" : "unknown");

  return 0;
}

