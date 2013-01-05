#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "atflocale.h"
#include "hash.h"
#include "npool.h"
#include "runexpat.h"

struct npool *sig_pool;
Hash_table *signiary;
int verbose;

static void
sH(void *userData, const char *name, const char **atts)
{
  const char *utf8 = findAttr(atts,"http://oracc.org/ns/gdl/1.0:utf8");
  if (*utf8)
    {
      static wchar_t wbuf[128];
      static size_t n, i;
      char xbuf[8];
      n = mbstowcs(wbuf,utf8,128);
      for (i = 0; i < n; ++i)
	{
	  static int one = 1;
	  sprintf(xbuf,"x%05X",wbuf[i]);
	  if (!hash_find(signiary,(unsigned char*)xbuf))
	    hash_add(signiary,npool_copy((unsigned char*)xbuf,sig_pool),(void*)&one);
	}
    }
}

static void
eH(void *userData, const char *name)
{
}

static void
printem(unsigned char *key,void *data)
{
  printf("%s\n",key);
}

int
main(int argc, char **argv)
{
  setlocale(LC_ALL,LOCALE);
  sig_pool = npool_init();
  signiary = hash_create(1000);
  runexpatNS(i_stdin,NULL,sH,eH,":");
  hash_exec2(signiary,printem);
  npool_term(sig_pool);
  return 1;
}

const char *prog = "signiary";
int major_version = 1, minor_version = 0;
const char *usage_string = "signiary <XTF >LST";
void help () { }
int opts(int arg,char*str){ return 1; }
