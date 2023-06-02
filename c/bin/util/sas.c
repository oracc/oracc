#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <psd_base.h>
#include <f2.h>
#include <fname.h>
#include <sas.h>
extern void usage(void);
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
do_alias(unsigned char *form, unsigned char *cf, unsigned char *gw, unsigned char *pos)
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
      fprintf(stderr,"input form %s aliases to %s\n",form,aform);
    }
  else
    {
      fprintf(stderr,"input form %s unchanged\n", form);
    }

  return ret;
}

int
main (int argc, char **argv)
{
  int ret = 0;
  sas_init();
  if (argv[1])
    {
      ret = do_alias((unsigned char *)argv[1], NULL, NULL, NULL);      
    }
  exit(ret);
}
void
help ()
{
}
const char *prog="sas";
const char *opts="";
const char *usage_string="FORM";
int major_version = 1, minor_version = 0;
