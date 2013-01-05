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
  struct skl *skltab;
  unsigned char *tab;
  struct reftab *refs;
  char *file;
};
static struct loaded_skl *skels = NULL;
static int nskels = 0;

Hash_table*
skl_load(const char *project, const char *skel, const char *type, const char*values)
{
  struct skl *skltab = NULL,*s;
  const char *file = NULL;
  FILE *f_skl = NULL;
  unsigned long ntab;
  const unsigned char *tab, *tabp;
  Hash_table *htab;
  static int data = 1;
  int i;
  struct reftab *refs = NULL;
  int refsindex = 0;
  int idrefs = 0;
  int skyext = !strcmp(type,"idref");

  if (NULL == (file = skl_file(project,skel,skyext)))
    return NULL;

  if (NULL == (f_skl = fopen(file,"r")))/*NO, don't use xfopen here...*/
    {
      if (strcmp(project,"cdli"))
	{
	  free((char*)file);
	  if (NULL == (file = skl_file("cdli",skel,skyext)))
	    return NULL;
	  if (NULL == (f_skl = fopen(file,"r")))
	    {
	      free((char*)file);
	      return NULL;
	    }
	}
      else
	{
	  return NULL;
	}
    }

  xfread(file,1,&ntab,4,1,f_skl);
  skltab = malloc(ntab * sizeof(struct skl));
  xfread(file,1,skltab,sizeof(struct skl),ntab,f_skl);
  for (s = skltab,i=0; i < ntab; ++i,++s)
    if (!strcmp(s->values,values) && !strcmp(s->type,type))
      break;
  if (i == ntab)
    {
      free(skltab);
      xfclose(file,f_skl);
      return NULL;
    }
  /*FIXME: should keep a list of these so they can be free'd */
  tab = tabp = malloc(s->length);
  xfseek(file,f_skl,s->offset,SEEK_SET);
  xfread(file,1,tab,1,s->length,f_skl);
  xfclose(file,f_skl);

  if (!strcmp(s->type,"idref"))
    {
      refs = calloc(s->count, sizeof(struct reftab));
      refsindex = 0;
      idrefs = 1;
    }

  htab = hash_create(s->count);
  hash_add(htab,(unsigned char *)"#url",(void*)file);
  if (idrefs)
    {
      for (i = 0; i < s->count; ++i)
	{
	  unsigned char *psep = (unsigned char *)strchr((char*)tabp,'\t');
	  unsigned char *idp;
	  struct reftab *rp;
	  *psep = '\0';
	  idp = psep+1;
	  rp = &refs[refsindex++];
	  rp->type = rt_raw;
	  rp->c.id = (const char *)idp;
	  idp[-1] = '\0';
	  /* fprintf(stderr,"adding idref %s...",tabp); */
	  hash_add(htab,tabp,rp);
	  /* fprintf(stderr,"ok\n"); */
	  tabp = idp + strlen((const char *)idp) + 1;
	}
    }
  else
    {
      for (i = 0; i < s->count; ++i)
	{
	  /* fprintf(stderr,"adding key %s\n",tabp); */
	  if (!hash_find(htab,tabp)) /*FIXME: should issue duplicate key diagnostic*/
	    hash_add(htab,tabp,&data);
	  tabp += strlen((const char *)tabp) + 1;	 
	}
    }

  skels = realloc(skels,(nskels+1)*sizeof(struct loaded_skl));
  skels[nskels].hash = htab;
  skels[nskels].refs = refs;
  skels[nskels].skltab = skltab;
  skels[nskels].tab = (unsigned char*)tab;
  skels[nskels].file = (char*)file;
  ++nskels;
  return htab;
}

void
skl_free(struct loaded_skl*sklp)
{
  free(sklp->tab);
  free(sklp->skltab);
  free(sklp->file);
  hash_free(sklp->hash,NULL);
  if (sklp->refs)
    free(sklp->refs);
  sklp->hash = NULL;
}

void
skl_unload(Hash_table *tabp)
{
  int i;
  for (i = 0; i < nskels; ++i)
    if (skels[i].hash == tabp)
      {
	skl_free(&skels[i]);
	return;
      }
}

void
skl_term()
{
  int i;
  for (i = 0; i < nskels; ++i)
    if (skels[i].hash)
      skl_free(&skels[i]);
  free(skels);
  skels = NULL;
  nskels = 0;
}
