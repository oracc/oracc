#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psd_base.h>
#include <hash.h>
#include <xsystem.h>
#include <skl.h>

struct loaded_skl
{
  Hash_table *hash;
  unsigned char *tab;
  char *cbd;
  char *file;
};
static struct loaded_skl *skels = NULL;
static int nskels = 0;

char *
cbd_ident(const char *project, const char *skel)
{
#if 0
  char buf[128];
  sprintf(buf,"%s/%s",project,skel);
#else
  char buf[128], *dash;
  /*  strcpy(buf,project); */
  if ((dash = strchr(skel,'-')))
    sprintf(buf,"%s/cbd/%s",project,dash+1);
  else
    /* this may make ID errors down the line, but at least we'll discover problems that
	 way...*/
    sprintf(buf,"%s/cbd/%s",project,skel);
#endif
  return xstrdup(buf);
}

Hash_table*
smp_load(const char *project, const char *skel)
{
  const char *file = NULL;
  FILE *f_skl = NULL;
  unsigned long nstr;
  const char *tab, *tabp;
  Hash_table *htab;
  int i;
  struct stat sbuf;
  char *cbdid;

  if (NULL == (file = skl_file_e(project,skel,"smp")))
    return NULL;

  if (NULL == (f_skl = fopen(file,"r")))
    {
      if (strcmp(project,"cdli"))
	{
	  free((char*)file);
	  if (NULL == (file = skl_file_e("cdli",skel,"smp")))
	    return NULL;
	  if (NULL == (f_skl = fopen(file,"r")))
	    {
	      free((char*)file);
	      return NULL;
	    }
	}
      else
	{
	  free((char*)file);
	  return NULL;
	}
    }

  stat(file,&sbuf);
  xfread(file,1,&nstr,4,1,f_skl);
  tab = tabp = malloc(sbuf.st_size - 4);
  xfseek(file,f_skl,4,SEEK_SET);
  xfread(file,1,tab,1,sbuf.st_size - 4,f_skl);
  xfclose(file,f_skl);

  htab = hash_create(nstr);
  hash_add(htab,(unsigned char *)"#cbd",cbdid = cbd_ident(project,skel));
  for (i = 0; i < nstr; ++i)
    {
      const char *data = tabp+strlen(tabp)+1;
      hash_add(htab,(unsigned char *)tabp,(void*)data);
      tabp = data + strlen(data) + 1;
    }

  skels = realloc(skels,(nskels+1)*sizeof(struct loaded_skl));
  skels[nskels].hash = htab;
  skels[nskels].tab = (unsigned char*)tab;
  skels[nskels].file = (char*)file;
  skels[nskels].cbd = cbdid;
  ++nskels;

  return htab;
}

void
smp_free(struct loaded_skl*smpp)
{
  free(smpp->tab);
  hash_free(smpp->hash,NULL);
  free(smpp->file);
  free(smpp->cbd);
  smpp->hash = NULL;
}

void
smp_unload(Hash_table *tabp)
{
  int i;
  for (i = 0; i < nskels; ++i)
    if (skels[i].hash == tabp)
      {
	smp_free(&skels[i]);
	return;
      }
}

void
smp_term()
{
  int i;
  for (i = 0; i < nskels; ++i)
    if (skels[i].hash)
      smp_free(&skels[i]);
  free(skels);
  skels = NULL;
  nskels = 0;
}
