#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype128.h>
#include <psd_base.h>
#include <fname.h>
#include "types.h"
#include "selib.h"

extern struct lm_tab *langmask(register const char *str, register unsigned int len);

const char *
attr_by_name(const char **atts,const char *name)
{
  int i;
  for (i = 0; atts[i] != NULL; i+=2)
    if (!strcmp(atts[i],name))
      return atts[i+1];
  return NULL;
}

int
lang_mask(const char **atts)
{
  const char *langsp = langs(atts);
  if (langsp)
    return strtoul(langsp,NULL,0);
  else
    return 0;
}

void
loc8(const char *id, int word, int lmask, struct location8 *l8p)
{
  l8p->text_id = l8p->unit_id = l8p->word_id = 0;
  l8p->text_id = atoi(id+1);
  if (*id == 'Q')
    setQ(l8p->text_id);
  else if (*id == 'X')
    {
#if 1
      l8p->text_id = XIFY_ID(l8p->text_id);
#else
      int orig = l8p->text_id;
      l8p->text_id = XIFY_ID(l8p->text_id);
      fprintf(stderr,"remapping text_id %c%d to %d=0x%x\n",*id,orig,l8p->text_id,l8p->text_id);
#endif
    }
  l8p->text_id |= lmask;
}

int
lang2mask(const char *l)
{
  struct lm_tab *tabp = langmask(l,strlen(l));
  if (tabp)
    return tabp->lm;
  else
    return lm_misc;
}

void
wid2loc8(const char *id, const char *lang, struct location8 *l8p)
{
  if (isupper(*id) || 'v' == *id)
    {
      l8p->text_id = atoi(id+1);
      if (*id == 'Q')
	setQ(l8p->text_id);
      else if (*id == 'X')
	l8p->text_id = XIFY_ID(l8p->text_id);
      id += 8;
      l8p->unit_id = atoi(id);
      while (*id && '.' != *id)
	++id;
      ++id;
      l8p->word_id = atoi(id);
      if (lang)
	l8p->text_id |= lang2mask(lang);
    }
  else
    {
      char *x = strchr(id,'.');
      if (x)
	{
	  l8p->text_id = atoi(x+2);
	  x += 2;
	}
      else
	fprintf(stderr,"wid2loc8: %s: malformed ID\n", id);
    }
}

const char *
se_dir(const char *project, const char *xname)
{
  static char _dirbuf[_MAX_PATH];
  sprintf(_dirbuf,"%s/pub/%s/%s",oracc_var(),project,xname);
  return _dirbuf;
}

const char *
se_file(const char *project, const char *xname, const char *fname)
{
  char *fbuf = (char*)se_dir(project,xname);
  strcat(fbuf,"/");
  return strcat(fbuf,fname);
}

void
ce_cfg(const char *project, const char *index,
       const char *ce_tag, const char *ce_ext, 
       enum ce_files ce_type, const char **proxies)
{
  const char *cfg_fn = se_file(project,index,"ce.cfg");
  FILE *cfg_fp = xfopen(cfg_fn,"w");
  fprintf(cfg_fp,"%d",ce_type);
  if (ce_ext)
    fprintf(cfg_fp," %s",ce_ext);
  fprintf(cfg_fp," %s%c",ce_tag,0);
  if (proxies)
    while (*proxies)
      fprintf(cfg_fp,"%s%c",*proxies++,0);
  fprintf(cfg_fp,"%c",0);
  xfclose(cfg_fn,cfg_fp);
}
