#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c1_list.h>
#include <lng.h>
#include "xcl.h"
#include "ilem_form.h"
#include "f2.h"
#include "npool.h"
#include "xli.h"
#include "xcl.h"

extern int bootstrap_mode, lem_extended;

#ifndef strdup
extern char *strdup(const char *);
#endif

static unsigned char *
sig_one(struct xcl_context *xcp, struct ilem_form *ifp, struct f2 *fp, int tail)
{
  unsigned char buf[1024];
  const unsigned char *form_to_use;
  int wild_form = 0;

  if (ifp && lem_extended)
    xli_ilem(xcp, ifp, fp);

  if (strstr((const char *)fp->lang, "-949"))
    wild_form = 1;

  if (!fp->project)
    fp->project = (const Uchar *)xcp->project;
  
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
      else if (ifp && ifp->fcount)
	{
	  fp->base = ifp->finds[0]->f2.base;
	  fp->cont = ifp->finds[0]->f2.cont;
	  fp->morph = ifp->finds[0]->f2.morph;
	}
      else
	fp->base = fp->cont = fp->morph = (Uchar*)"X";
    }

  if (!fp->norm)
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

unsigned char *
f2_sig(struct xcl_context *xcp, struct ilem_form *ifp, struct f2 *fp)
{
  unsigned char *ret = NULL;
  struct npool *pool = xcp->pool;

  if (!fp)
    return NULL;

  if (fp->parts)
    {
      unsigned char *tmp = NULL;
      tmp = sig_one(xcp, ifp, fp, 0);
      if (tmp) 
	{
	  List *parts = list_create(LIST_SINGLE);
	  int i;
	  list_add(parts, tmp);
	  for (i = 0; fp->parts[i]; ++i)
	    {
	      fp->parts[i]->tail_sig = tmp = sig_one(xcp, ifp, fp->parts[i], 1);
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
      tmp = sig_one(xcp, ifp, fp, 0);
      ret = npool_copy(tmp,pool);
      free(tmp);
    }

  return ret;
}

#if 0
static void
append_sig_sans_form(unsigned char *buf,unsigned char *sig)
{
  unsigned char *dst = buf+strlen((char*)buf);
  while (*sig && ':' != *sig)
    *dst++ = *sig++;
  *dst++ = *sig++;
  while (*sig && '=' != *sig)
    ++sig;
  while (*sig)
    *dst++ = *sig++;
  *dst = '\0';
}
#endif

static char *
tabless(const unsigned char *s)
{
  static char buf[1024], *tab;
  strcpy(buf,(char*)s);
  tab = strchr(buf,'\t');
  if (tab)
    *tab = '\0';
  return buf;
}

unsigned char *
f2_psu_sig(struct xcl_context *xcp, struct f2 *fp)
{
  unsigned char psu_buf[2048];

  if (fp->parts)
    { 
      int i;
      char *amp = NULL;
      unsigned char psu_form[1024], buf[2048];

      *psu_form = *buf = '\0';

      for (i = 0; fp->parts[i]; ++i)
        {
	  /* New PSU form generation */
	  if (fp->parts[i]->cof_id)
	    {
	      if (!fp->parts[i]->tail_sig)
		{
		  if (BIT_ISSET(fp->flags, F2_FLAGS_NEW_BY_PROJ)
		      || BIT_ISSET(fp->flags, F2_FLAGS_NEW_BY_LANG))
		    strcat((char*)psu_form, (char*)fp->parts[i]->form);
		  else if (fp->parts[i]->oform)
		    strcat((char*)psu_form, (char*)fp->parts[i]->oform);
		  else
		    strcat((char*)psu_form, (char*)fp->parts[i]->form);
		}
	    }
	  else
	    {
	      if (BIT_ISSET(fp->flags, F2_FLAGS_NEW_BY_PROJ)
		       || BIT_ISSET(fp->flags, F2_FLAGS_NEW_BY_LANG))
		strcat((char*)psu_form, (char*)fp->parts[i]->form);
	      else if (fp->parts[i]->oform)
		strcat((char*)psu_form, (char*)fp->parts[i]->oform);
	      else
		strcat((char*)psu_form, (char*)fp->parts[i]->form);
	    }

	  strcat((char*)psu_form, " ");

          if (i)
	    strcat((char*)buf, "++");

          if (fp->parts[i]->tail_sig)
            strcat((char*)buf, (char*)fp->parts[i]->tail_sig);
          else
            {
              if (!fp->parts[i]->sig)
                /* passing NULL as arg2 means do not run the extended lemmatization--this is ok
                   because all the parts will have been subjected to that anyway by now */
                fp->parts[i]->sig = f2_sig(xcp, NULL, fp->parts[i]);

              if ((amp = strstr((char*)fp->parts[i]->sig, "&&")))
                {
                  int len = strlen((char*)buf) + (amp - (char*)fp->parts[i]->sig);
                  strncat((char*)buf, (char*)fp->parts[i]->sig, amp - (char*)fp->parts[i]->sig);
                  buf[len] = '\0';
                }
              else
                strcat((char*)buf,tabless(fp->parts[i]->sig));
            }
        }
      sprintf((char*)psu_buf,"{%s= %s}::%s",psu_form,fp->psu_ngram,buf);
    }
  else
    sprintf((char*)psu_buf,"{%s}::",fp->psu_ngram);

  return npool_copy(psu_buf,xcp->pool);
}
