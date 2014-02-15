#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "atflocale.h"
#include "gsl.h"
#include "hash.h"
#include "npool.h"
#include "runexpat.h"

char curr_project[1024], curr_text_id[8];
struct npool *sig_pool;
Hash_table *signiary, *pertext, *curr_hash = NULL, *curr_sig_hash = NULL;
int total_sign_instances = 0, verbose;

static void
incr_val(Hash_table *h, const unsigned char *v)
{
  int *counter = NULL;
  if (!(counter = hash_find(h, v)))
    {
      counter = malloc(sizeof(int *));
      *counter = 1;
      hash_add(h, npool_copy(v, sig_pool), counter);
    }
  else
    *counter += 1;
}

static void
sH(void *userData, const char *name, const char **atts)
{
  
  if (name[22] == 'f'
      && (!strcmp(name, "http://oracc.org/ns/xtf/1.0:transliteration")
	  || !strcmp(name, "http://oracc.org/ns/xtf/1.0:composite")))
    {
      strcpy(curr_project, findAttr(atts,"project"));
      strcpy(curr_text_id, get_xml_id(atts));
    }
  else
    {
      const char *utf8 = findAttr(atts,"http://oracc.org/ns/gdl/1.0:utf8");
      if (*utf8)
	{
	  static wchar_t wbuf[128];
	  static size_t n, i;
	  char sbuf[512], xbuf[1024], *hex;
	  unsigned char *sn = NULL;
	  const char *form = NULL;
	  
	  n = mbstowcs(wbuf,utf8,128);
	  hex = malloc(n * 8);
	  *hex = '\0';
	  for (i = 0; i < n; ++i)
	    {
	      if (i)
		strcat(hex, ".");
	      sprintf(hex+strlen(hex),"x%05X",wbuf[i]);
	    }
	  if (!(sn = psl_hex_to_sign(hex)))
	    sn = (unsigned char *)hex;
	  sprintf(sbuf,"%s:%s",hex,sn);
	  sprintf(xbuf,"%s:%s:%s:%s",curr_project,curr_text_id,hex,sn);
	  free(hex);
	  
	  if (!(curr_sig_hash = hash_find(signiary,(unsigned char*)sbuf)))
	    {
	      curr_sig_hash = hash_create(1);
	      hash_add(signiary,npool_copy((unsigned char*)sbuf,sig_pool),curr_sig_hash);
	    }
	  incr_val(curr_sig_hash, (const unsigned char *)"#count");
	  ++total_sign_instances;

	  if (!(curr_hash = hash_find(pertext,(unsigned char*)xbuf)))
	    {
	      curr_hash = hash_create(1);
	      hash_add(pertext,npool_copy((unsigned char*)xbuf,sig_pool),curr_hash);
	    }
	  incr_val(curr_hash, (const unsigned char *)"#count");

	  form = findAttr(atts, "form");
	  if (form && *form)
	    {
	      incr_val(curr_sig_hash, (const unsigned char *)form);
	      incr_val(curr_hash, (const unsigned char *)form);
	      curr_sig_hash = curr_hash = NULL;
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
      incr_val(curr_sig_hash, val);
      incr_val(curr_hash, val);
      curr_sig_hash = curr_hash = NULL;
    }
  else
    charData_discard();
}

static void
printvals(const unsigned char *key, void *data, void *user)
{
  if (*key != '#')
    {
      int ipct = 0;
      ipct = pct(*(int*)data, *(int*)user);
      printf("%s %d/%d\t",key, *(int*)data, ipct);
    }
}

static void
printsigns(const unsigned char *key,void *data)
{
  static int sign_count = 0;
  int ipct = 0;
  sign_count = *(int*)(hash_find(data,(const unsigned char *)"#count"));
#if 1
  ipct = pct(sign_count,signiary->key_count);
#else
  ipct = pct(sign_count,total_sign_instances);
#endif
  printf("%s\t%d\t%d\t",key,sign_count,ipct);
  hash_exec_user_key_data(data, printvals, &sign_count);
  printf("\n");
}

int
main(int argc, char **argv)
{
  setlocale(LC_ALL,LOCALE);
  sig_pool = npool_init();
  signiary = hash_create(1000);
  pertext = hash_create(1000);
  psl_init();
  runexpatNS(i_stdin,NULL,sH,eH,":");
  hash_exec2(signiary, printsigns);
  npool_term(sig_pool);
  psl_term();
  return 1;
}

const char *prog = "signiary";
int major_version = 1, minor_version = 0;
const char *usage_string = "signiary <XTF >LST";
void help () { }
int opts(int arg,char*str){ return 1; }
