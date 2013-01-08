#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <bit_mac.h>

unsigned char *catvec;
size_t max_cat_id;

unsigned char *loadfile(unsigned const char *fname, size_t *nbytes);

void
catvec_init()
{
  size_t vecsize;
  catvec = loadfile((unsigned const char *)"/usr/local/oracc/lib/data/catpnums.vec",&vecsize);
  max_cat_id = vecsize * 8;
}

void
catvec_term()
{
  free(catvec);
  catvec = NULL;
}

void
catvec_dump()
{
  int i;
  for (i = 0; i < max_cat_id; ++i)
    {
      int v = i/8;
      int b = (1<<(i%8));
      if (BIT_ISSET(catvec[v],b))
	fprintf(stderr,"P%06d\n",i);
    }
}
