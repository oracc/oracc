#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "atflocale.h"
#include "gsl.h"
#include "hash.h"
#include "npool.h"
#include "runexpat.h"

const char *curr_project = NULL, *curr_text_id = NULL;
struct npool *sig_pool;
Hash_table *signiary, *curr_hash = NULL;
int verbose;

static void
sH(void *userData, const char *name, const char **atts)
{
  
  if (!strcmp(name, "http://oracc.org/ns/xtf/1.0:transliteration"))
    {
      curr_project = findAttr(atts,"project");
      curr_text_id = get_xml_id(atts);
    }
  else
    {
      const char *utf8 = findAttr(atts,"http://oracc.org/ns/gdl/1.0:utf8");
      if (*utf8)
	{
	  static wchar_t wbuf[128];
	  static size_t n, i;
	  char xbuf[1024];
	  n = mbstowcs(wbuf,utf8,128);
	  for (i = 0; i < n; ++i)
	    {
	      char hex[8];
	      unsigned char *sn = NULL;
	      sprintf(hex,"x%05X",wbuf[i]);
	      if (!(sn = psl_hex_to_sign(hex)))
		sn = (unsigned char *)hex;
	      sprintf(xbuf,"%s:%s:%s",curr_project,curr_text_id,sn);
	      if (!(curr_hash = hash_find(signiary,(unsigned char*)xbuf)))
		{
		  curr_hash = hash_create(1);
		  hash_add(signiary,npool_copy((unsigned char*)xbuf,sig_pool),curr_hash);
		}
	    }
	}
    }
}

static void
eH(void *userData, const char *name)
{
  if (curr_hash)
    {
      unsigned char *val = (unsigned char *)charData_retrieve();
      int *counter = NULL;
      if (!(counter = hash_find(curr_hash, val)))
	{
	  counter = malloc(sizeof(int *));
	  *counter = 1;
	  hash_add(curr_hash, npool_copy(val, sig_pool), counter);
	}
      else
	*counter += 1;
    }
  else
    charData_discard();
  curr_hash = NULL;
}

static void
printvals(unsigned char *key,void *data)
{
  printf("%s %d\t",key, *(int*)data);
}

static void
printsigns(unsigned char *key,void *data)
{
  printf("%s\t",key);
  hash_exec2(data, printvals);
  printf("\n");
}

int
main(int argc, char **argv)
{
  setlocale(LC_ALL,LOCALE);
  sig_pool = npool_init();
  signiary = hash_create(1000);
  psl_init();
  runexpatNS(i_stdin,NULL,sH,eH,":");
  hash_exec2(signiary,printsigns);
  npool_term(sig_pool);
  psl_term();
  return 1;
}

const char *prog = "signiary";
int major_version = 1, minor_version = 0;
const char *usage_string = "signiary <XTF >LST";
void help () { }
int opts(int arg,char*str){ return 1; }
