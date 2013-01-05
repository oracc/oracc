#include <string.h>
#include "gsl.h"
#include "sas.h"
#include "f2.h"
/*
  ref_fp is the f2 we use as the CFGWPOS source; fp is the f2 that is being
  processed by the lemmer; if ref_fp is NULL fp is used as ref_fp as well
 */
int
f2_alias(struct sig_context *scp, struct f2 *fp, struct f2 *ref_fp)
{
  int ret = 0;
  if (!ref_fp)
    ref_fp = fp;
  if (scp->aliases)
    {
      unsigned char *aform = NULL;
      aform = sas_alias_form(scp->aliases, 
			     fp->form,
			     ref_fp->cf,
			     ref_fp->gw,
			     ref_fp->pos);
      if (strcmp((char*)fp->form,(char*)aform))
	{
	  fp->oform = fp->form;
	  fp->form = npool_copy(aform,scp->pool);
	  ret = 1;
	  if (verbose > 1)
	    fprintf(stderr,"aliased form %s => fp->form %s\n",fp->oform,fp->form);
	}
      free(aform);
    }
  return ret;
}

int
f2_extreme_alias(struct sig_context *scp, struct f2 *fp, struct f2 *ref_fp)
{

  if (!f2_form_signs(fp->form, ref_fp->form))
    return 0;

  fp->oform = fp->form;
  fp->form = npool_copy(ref_fp->form,scp->pool);

  if (verbose > 1)
    fprintf(stderr,"extreme aliased form %s => fp->form %s\n",fp->oform,fp->form);

  return 1;
}

int
f2_form_signs(const unsigned char *f1, const unsigned char *f2)
{
  int i;
  int m1len = 0, m2len = 0;
  struct sas_map *m1 = NULL, *m2 = NULL;

  m1 = sas_map_form(f1, &m1len);
  m2 = sas_map_form(f2, &m2len);

  if (m1len != m2len)
    return 0;

  gsl_map_ids(m1);
  gsl_map_ids(m2);

  for (i = 0; m1[i].v; ++i)
    if (strcmp((const char *)m1[i].a, (const char *)m2[i].a))
      return 0;

  return 1;
}
