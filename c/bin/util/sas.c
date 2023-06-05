#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/unistd.h>
#include <psd_base.h>
#include <f2.h>
#include <fname.h>
#include <sas.h>
#include <hash.h>
#include <npool.h>

extern void usage(void);

extern int options(int, char**,const char*);
void help(void);
extern int optind;

int lnum = 0;

struct npool *sig_pool = NULL;
Hash_table *sig_hash = NULL;
Hash_table *aliases_hash = NULL;
List *sig_list = NULL;

int sigs = 0;
int status = 0;
int stdinput = 0;
int twocol = 0;
int verbose = 0;

struct sas_info *aliases = NULL;

void
sas_init(void)
{
  char buf[1024];
  char *aliases_file = NULL;
  sprintf(buf,"%s/pub/epsd2/alias-smart.txt",oracc_home());
  aliases_file = buf;
  if (!xaccess(buf,R_OK,0))
    {
      aliases = sas_asa_load(buf);
      if (aliases)
	{
	  if (verbose)
	    fprintf(stderr,"sas: using aliases %s\n",aliases_file);
	}
      else
	fprintf(stderr,"sas: failed to load readable aliases %s\n",aliases_file);
    }
}

int
do_alias(unsigned char *form, unsigned char *cf, unsigned char *gw, unsigned char *pos,
	 unsigned char **ap)
{
  unsigned char *aform = NULL;
  int ret = 0;
  aform = sas_alias_form(aliases,
			 form,
			 cf,
			 gw,
			 pos);
  if (strcmp((char*)form,(char*)aform))
    {
      ret = 1;
      if (verbose)
	fprintf(stderr,"input form %s aliases to %s\n",form,aform);
    }
  else
    {
      if (verbose)
	fprintf(stderr,"input form %s unchanged\n", form);
    }
  *ap = aform;
  return ret;
}

int
do_alias_sig(unsigned char *sig, unsigned char **ap)
{
  static int one = 1;
  int ret = 0;
  unsigned const char *keepsig = NULL;
  unsigned char *form = NULL, *cf = NULL, *gw = NULL, *pos = NULL;

  if (!(keepsig = hash_exists(sig_hash, sig)))
    hash_add(sig_hash, (keepsig = npool_copy(sig, sig_pool)), &one);

  if ((form = (unsigned char *)strchr((const char *)sig, ':')))
    {
      *form++ = '\0';
      if ((cf = (unsigned char *)strchr((const char *)form, '=')))
	{
	  *cf++ = '\0';
	  if ((gw = (unsigned char *)strchr((const char *)cf, '[')))
	    {
	      *gw++ = '\0';
	      if ((pos = (unsigned char *)strchr((const char *)gw, ']')))
		{
		  unsigned char *e = NULL;
		  *pos++ = '\0';
		  e = pos;
		  while (isupper(*e))
		    ++e;
		  if ('/' == *e && ('i' == e[1] || 't' == e[1]))
		    e += 2;
		  *e = '\0';
		}
	    }
	}
      list_add(sig_list, (unsigned char*)keepsig);
      /* need to implement wrapper for aliasing PSUs */
      if ('{' != *keepsig && (ret = do_alias(form, cf, gw, pos, ap)))
	{
	  unsigned char *sigalias = NULL, *postform = NULL;
	  postform = (unsigned char *)strchr((const char *)keepsig, '=');
	  sigalias = malloc(strlen((const char*)keepsig)
			    + strlen((const char *)(*ap))
			    + strlen((const char *)postform)
			    + 3);
	  (void)sprintf((char *)sigalias, "+%s:%s%s", sig, *ap, postform);
	  if (verbose)
	    fprintf(stderr, "%s =>\n\t%s\n", keepsig, sigalias);
	  list_add(sig_list, sigalias);
	}      
    }
  else if (strncmp((const char *)sig, "@fields", strlen("@fields")))
    fprintf(stderr, "%d: no form in sig %s\n", lnum, sig);
  
  return ret;
}

void
do_one(unsigned char *form)
{
  static unsigned char *aform = NULL;
  int ret = 0;
  if (sigs)
    ret = do_alias_sig(form, &aform);
  else
    {
      ret = do_alias(form, NULL, NULL, NULL, &aform);
      if (twocol)
	printf("%s\t%s\n", form, aform);
      else
	printf("%s\n", aform);
      fflush(stdout);
    }
  status += ret;
}

int
main (int argc, char **argv)
{
  options(argc, argv, "lsv2");

  sas_init();

  if (sigs)
    {
      aliases_hash = hash_create(1024);
      sig_hash = hash_create(1024);
      sig_pool = npool_init();
      sig_list = list_create(LIST_SINGLE);
    }
  
  if (stdinput)
    {
      char *lp = NULL, buf[2048];
      while (NULL != (lp = fgets(buf,2048,stdin)))
	{
	  ++lnum;
	  if ('\n' == buf[strlen(buf)-1])
	    buf[strlen(buf)-1] = '\0';
	  do_one((unsigned char *)buf);
	}
      if (!feof(stdin))
	fprintf(stderr, "error reading input at line %d\n", lnum);
    }
  else
    {
      if (argv[optind])
	{
	  do_one((unsigned char *)argv[optind]);
	}
      else
	help();
    }

  if (sigs)
    {
      unsigned char *s = NULL;
      for (s = list_first(sig_list); s; s = list_next(sig_list))
	{
	  if ('+' == *s)
	    {
	      ++s;
	      if (!hash_find(sig_hash, s))
		puts((char*)s);
	    }
	  else
	    puts((char*)s);
	}
    }
  
  exit(status);
}
void
help ()
{
}
const char *prog="sas";
int opts(int argc, char *arg)
{
  switch (argc)
    {
    case 'l': stdinput=1; break;
    case 's': sigs=stdinput=1; break;
    case 'v': verbose=1; break;
    case '2': twocol=1; break;
    default: return 1;
    }
  return 0;
}
const char *usage_string="FORM";
int major_version = 1, minor_version = 0;
