#include <ctype128.h>
#include "gx.h"

static unsigned char *generic_sig(struct npool *pool, struct f2 *fp);
static unsigned char *sig_one(struct npool *pool, struct f2 *fp, int tail);

int xnorms = 0; /* set to 1 to produce $X for empty norms */

unsigned char *
form_sig(struct entry *e, struct f2 *f2p)
{
  return generic_sig(e->owner->pool, f2p);
}

static unsigned char *
sig_one(struct npool *pool, struct f2 *fp, int tail)
{
  unsigned char buf[1024];
  const unsigned char *form_to_use;
  int wild_form = 0;

  if (strstr((const char *)fp->lang, "-949"))
    wild_form = 1;

  if (BIT_ISSET(fp->flags, F2_FLAGS_NORM_IS_CF)) 
    fp->cf = fp->norm;

  if (BIT_ISSET(fp->flags, F2_FLAGS_NEW_BY_PROJ)
      || BIT_ISSET(fp->flags, F2_FLAGS_NEW_BY_LANG))
    form_to_use = fp->form;
  else
    form_to_use = (Uchar*)(wild_form 
			   ? "*" 
			   : (fp->oform ? (char*)fp->oform : (char*)fp->form));

  if (!fp->cf && fp->pos && !strcmp((char *)fp->pos,"n"))
    {
      fp->cf = (const Uchar *)"n";
      fp->gw = fp->sense = (const Uchar *)"n";
      fp->epos = (const Uchar *)"n";
    }
  
  sprintf((char*)buf,"@%s%%%s:%s=%s[%s//%s]%s'%s",
	  (char*)(fp->project),
	  (char*)(fp->lang),
	  (char*)(tail ? (Uchar*)"" : form_to_use),
	  (char*)(fp->cf ? fp->cf : (Uchar*)"X"),
	  (char*)(fp->gw ? fp->gw : (Uchar*)"X"),
	  (char*)(fp->sense ? fp->sense : (Uchar*)"X"),
	  (char*)(fp->pos ? fp->pos : (Uchar*)"X"),
	  (char*)(fp->epos ? fp->epos : (Uchar*)"X"));

  if (BIT_ISSET(fp->core->features,LF_BASE)
      && !fp->base)
    {
      if (fp->pos && !strcmp((char *)fp->pos, "n"))
	{
	  fp->base = fp->form;
	  fp->morph = (const Uchar *)"~";
	  fp->cont = (const Uchar *)"";
	  fp->norm = (const Uchar *)"n";
	}
      else
	fp->base = fp->cont = fp->morph = (Uchar*)"X";
    }

  if (!fp->norm && xnorms)
    fp->norm = (Uchar*)"X";

  if (fp->norm)
      sprintf((char*)(buf+strlen((char*)buf)),"$%s",fp->norm);

  if (fp->base)
    sprintf((char*)(buf+strlen((char*)buf)),"/%s",fp->base);

  if (fp->cont && *fp->cont)
    sprintf((char*)(buf+strlen((char*)buf)),"+%s",fp->cont);

  if (fp->morph)
    sprintf((char*)(buf+strlen((char*)buf)),"#%s",fp->morph);

  if (fp->morph2)
    sprintf((char*)(buf+strlen((char*)buf)),"##%s",fp->morph2);

  if (fp->stem)
    sprintf((char*)(buf+strlen((char*)buf)),"*%s",fp->stem);

  if (fp->rws)
    sprintf((char*)(buf+strlen((char*)buf)),"@%s",fp->rws);

  return (unsigned char *)strdup((char*)buf);
}

static unsigned char *
generic_sig(struct npool *pool, struct f2 *fp)
{
  unsigned char *ret = NULL;

  if (!fp)
    return NULL;

  if (fp->parts)
    {
      unsigned char *tmp = NULL;
      tmp = sig_one(pool, fp, 0);
      if (tmp) 
	{
	  List *parts = list_create(LIST_SINGLE);
	  int i;
	  list_add(parts, tmp);
	  for (i = 0; fp->parts[i]; ++i)
	    {
	      fp->parts[i]->tail_sig = tmp = sig_one(pool, fp->parts[i], 1);
	      if (tmp)
		list_add(parts, tmp);
	      else
		return NULL;
	    }
	  tmp = list_to_str2(parts, "&&");
	  ret = npool_copy(tmp,pool);
	  free(tmp);
	}
      else
	return NULL;
    }
  else
    {
      unsigned char *tmp = NULL;
      tmp = sig_one(pool, fp, 0);
      ret = npool_copy(tmp,pool);
      free(tmp);
    }

  return ret;
}
