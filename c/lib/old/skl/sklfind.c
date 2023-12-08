#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psd_base.h>
#include <hash.h>
#include <skl.h>

const char *sky_good[]  = { "abarakku" , NULL };
const char *sky_bad[]   = { "#lemmata#abarakku" , NULL };
const char *sky_ambig[] = { "adaru" , NULL };
const char *sky_multi[] = { "adaru#senses#be", 
			    "adaru#senses#dark", 
			    NULL };

#define sky_ret(ok,tried)	     \
  if (ok == (const char *)1) \
    fprintf(stderr,"%s ambiguous\n",tried);		\
  else if (ok) \
    fprintf(stderr,"%s ok\n",tried);			\
  else \
    fprintf(stderr,"%s not found\n",tried)

int
main(int argc, const char **argv)
{
  Hash_table *hp = skl_load("cdli","pcsl","simple","signs");
  const void *ok = hash_find(hp,(unsigned char *)"A");
  const char **kp;
  const char *ok2;
  if (ok)
    fprintf(stderr,"a ok\n");
  else
    fprintf(stderr,"a not found\n");
  hp = skl_load("epad","epad","idref","keys");
  kp = hash_keys(hp);
  ok = sky_find(hp,sky_bad); sky_ret(ok,sky_bad[0]);
  ok = sky_find(hp,sky_ambig); sky_ret(ok,sky_ambig[0]);
  ok = sky_find(hp,sky_multi); sky_ret(ok,sky_multi[1]);
  ok = sky_find(hp,sky_good); sky_ret(ok,sky_good[0]);
  hp = smp_load("epad","epad");
  ok2 = hash_find(hp,ok);
  if (ok2)
    fprintf(stderr,"%s => %s\n", (char*)ok, ok2);
  else
    fprintf(stderr,"%s not found in map\n", (char*)ok);
  return 0;
}
const char *prog = "sklfind";
int major_version = 1, minor_version = 0;
const char *usage_string = "";
void
help ()
{
}
void
opts ()
{
}
