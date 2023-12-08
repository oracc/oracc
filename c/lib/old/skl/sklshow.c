#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <psd_base.h>
#include <options.h>
#include <hash.h>
#include <skl.h>

const char *argv_project;
const char *argv_skeleton;

void
sky_dump(const char *project, const char *skel, const char *values, const char *type)
{
  struct skl *skltab = NULL,*s;
  const char *file = NULL;
  FILE *f_skl = NULL;
  unsigned long ntab;
  const unsigned char *tab, *tabp;
  int i;
  int skyext = 1;

  if (NULL == (file = skl_file(project,skel,skyext)))
    {
      fprintf(stderr,"sklshow: %s/%s.sky not found\n",project,skel);
      return;
    }
  if (NULL == (f_skl = fopen(file,"r")))
    {
      fprintf(stderr, "sklshow: %s found but not openable\n", file);
      return;
    }
  xfread(file,1,&ntab,4,1,f_skl);
  skltab = malloc(ntab * sizeof(struct skl));
  xfread(file,1,skltab,sizeof(struct skl),ntab,f_skl);
  for (s = skltab,i=0; i < ntab; ++i,++s)
    if (!strcmp(s->values,values) && !strcmp(s->type,type))
      break;
  if (i == ntab)
    {
      fprintf(stderr, "sklshow: values/type %s/%s not found in %s/%s.sky\n",
	      values, type, project, skel);
      return;
    }

  /*FIXME: should keep a list of these so they can be free'd */
  tab = tabp = malloc(s->length);
  xfseek(file,f_skl,s->offset,SEEK_SET);
  xfread(file,1,tab,1,s->length,f_skl);
  xfclose(file,f_skl);

  for (i = 0; i < s->count; ++i)
    {
      printf("%s\n",tabp);
      tabp += strlen((const char *)tabp) + 1;
    }
}

int
main(int argc, char *const *argv)
{
  options(argc,argv,"p:s:");
  sky_dump(argv_project,argv_skeleton, "keys", "idref");
  return 0;
}

const char *prog = "sklshow";
int major_version = 1, minor_version = 0;
const char *usage_string = "";
void
help ()
{
}
int
opts(int ch, char *arg)
{
  switch (ch)
    {
    case 'p':
      argv_project = arg;
      break;
    case 's':
      argv_skeleton = arg;
      break;
    default:
      return 1;
    }
  return 0;
}
