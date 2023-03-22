#include <stdlib.h>
#include <tree.h>

#include "gvl.h"

/* This interface is used from a Node with name=g:q */
gvl_gg*
gvl_vq_node(Node *np)
{
  gvl_g *qv = NULL;
  gvl_vq_gg(np->kids->parsed, np->kids->next->parsed, qv);
  return qv;
}

/* This interface is used when gvl_validate is passed a vq, e.g., e₄(A) */
void
gvl_vq(unsigned const char *g, gvl_g *gp)
{
  g = gvl_tmp_key(g,"qv");
  if ((l = gvl_lookup(g)))
    {
      gp->oid = (ccp)l;
      gp->sign = gvl_lookup(gvl_tmp_key(l,""));
    }
  else
    {
      static unsigned char *mess = NULL;
      (void)gvl_q_c10e(gp, &mess);
      if (mess)
	gp->mess = gvl_vmess("%s", mess);
      mess = NULL;
    }
}

static int
gvl_vq_c10e(gvl_g *gp, unsigned char **mess)
{
  gvl_g *vp = NULL, *qp = NULL;
  unsigned const char *v, *q;
  unsigned char *tmp = malloc(strlen((ccp)gp->text)+1), *end = NULL;
  int pnest = 0, v_bad = 0, q_bad = 0, ret = 0;

  /* get pointers to the value (v) and qualifier (q) parts */
  strcpy((char*)tmp, (ccp)gp->text);
  end = tmp+strlen((ccp)tmp);
  --end;
  *end = '\0';
  while (end > tmp && ('(' != end[-1] || pnest))
    {
      --end;
      if (')' == *end)
	++pnest;
      else if ('(' == *end)
	--pnest;
    }
  if (end == tmp)
    {
      *mess = gvl_vmess("[vq] %s: syntax error in value-qualifier", gp->text);
      return 0;
    }
  
  q = end--;
  *end = '\0';
  v = tmp;

  /* check the value */
  vp = gvl_validate(v);
      
  /* check the sign */
  qp = gvl_validate(q);

  return gvl_vq_gg(vp, qp, NULL);
}

int
gvl_vq_gg(gvl_g *vp, gvl_g *qp, gvl_g *vq)
{
  int v_bad = 0, q_bad = 0, ret = 0;
  unsigned char *q_fixed = NULL;

  if (vp)
    {
      if (vp->mess && !strstr((ccp)vp->mess, "must be qualified"))
	v_bad = 1;
    }
  else
    return NULL;

  if (qp)
    {
      if (!qp->sign)
	q_bad = 1;
      else if (strcmp((ccp)q, (ccp)qp->sign))
	{
	  q_fixed = malloc(strlen((ccp)q) + strlen((ccp)qp->sign) + strlen(" [ <= ] "));
	  (void)sprintf((char*)q_fixed," [%s <= %s]", qp->sign, q);
	}
    }
  else
    return NULL;

  /* Now if we have bad value and qualifier it's too hard to guess */
  if (v_bad && q_bad)
    *mess = gvl_vmess("[vq] %s: value %s and qualifier %s unknown%s", gp->text, vp->text, qp->text, QFIX);
  else if (v_bad)
    {
      /* If the v is unknown, check if the base is known for q under a different index, else report known v for q */
      int qv_bad = gvl_try_h(gp, vp, qp, q_fixed, mess);
      if (qv_bad)
	{
	  unsigned const char *tmp2 = gvl_lookup(gvl_tmp_key((uccp)qp->oid, "values"));
	  if (tmp2)
	    *mess = gvl_vmess("[vq] %s: %s::%s unknown. Known for %s: %s%s", gp->text, vp->text, qp->sign, qp->sign, tmp2, QFIX);
	  else
	    *mess = gvl_vmess("[vq] %s: %s::%s unknown. No known values for %s%s", gp->text, vp->text, qp->sign, qp->sign, QFIX);
	}
    }
  else if (q_bad)
    {
      /* If the q is unknown, report known q for v */
      unsigned const char *tmp2 = gvl_lookup(gvl_tmp_key(vp->text, "q"));
      if (tmp2)
	*mess = gvl_vmess("[vq] %s: q %s unknown: known for %s: %s%s", gp->text, q, vp->text, tmp2, QFIX);
      else if (!strchr((ccp)q,'X'))
	*mess = gvl_vmess("[vq] %s: q %s unknown: %s known as %s%s", gp->text, q, vp->text, vp->sign, QFIX);
    }
  else
    {
      unsigned char *tmp2 = malloc(strlen((ccp)vp->text) + strlen((ccp)qp->sign) + 3);
      sprintf((char*)tmp2, "%s(%s)", vp->text, qp->sign);
	  
      /* tmp2 is now a vq with valid v and q components */
      if (gvl_lookup(gvl_tmp_key(tmp2,"qv")))
	{
	  /* vq is known combo -- we have canonicalized the g that was passed as arg1 */
	  gp->oid = qp->oid;
	  gp->sign = gvl_lookup(gvl_tmp_key((uccp)qp->oid,""));
	  /* add gp->text to g hash as key of tmp2 ? */
	  if (gvl_strict)
	    *mess = gvl_vmess("[vq] %s: should be %s%s", gp->text, tmp2, QFIX);
	  ret = 1;
	}
      else if ('s' == *vp->type || 'c' == *vp->type)
	{
	  /* This is a qualified uppercase value like TA@g(LAK654a) */
	  if (strcmp(vp->oid, qp->oid))
	    {
	      unsigned const char *parents = gvl_lookup(gvl_tmp_key((uccp)qp->oid,"parents"));
	      if (parents)
		{
		  if (!strstr((ccp)parents, vp->oid))
		    {
		      unsigned char *snames = snames_of(parents);
		      *mess = gvl_vmess("[vq] %s: bad qualifier: %s is a form of %s%s", gp->text, qp->sign, snames, QFIX);
		      free(snames);
		    }
		}
	      else
		*mess = gvl_vmess("[vq] %s: value and qualifier are different signs (%s vs %s)%s",
				  gp->text, vp->sign, qp->sign, QFIX);
	    }
	}
      else
	{
	  /* is vq a v that doesn't need qualifying? */
	  if (vp->oid && qp->oid && !strcmp(vp->oid, qp->oid))
	    {
	      gp->oid = qp->oid;
	      gp->sign = gvl_lookup(gvl_tmp_key((uccp)qp->oid,""));
	      if (gvl_strict)
		*mess = gvl_vmess("[vq] %s: unnecessary qualifier on value%s", gp->text, QFIX);
	      ret = 1; /* this is still OK--we have resolved the issue deterministically */
	    }
	  else
	    {
	      /* if the qv is unknown see if the value has the wrong index in the q context */
	      int qv_bad = gvl_try_h(gp, vp, qp, q_fixed, mess);
	      if (qv_bad)
		{
		  /* we know the q doesn't have a value which is correct except for the index;
		     report known q for v */
		  if ('v' == *vp->type)
		    {
		      unsigned const char *q_for_v = gvl_lookup(gvl_tmp_key(v, "q"));
		      if (q_for_v)
			*mess = gvl_vmess("[vq] %s: unknown. Known for %s: %s%s", gp->text, vp->text, q_for_v, QFIX);
		      else
			*mess = gvl_vmess("[vq] %s: %s is %s%s", gp->text, vp->text, vp->sign, QFIX);
		    }
#if 1
		  else
		    {
		      fprintf(stderr, "gvl: [vq] unhandled case for input %s\n", gp->text);
		    }
#else
		  else
		    {
		      unsigned const char *parents = gvl_lookup(gvl_tmp_key((uccp)qp->oid,"parents"));
		      if (parents)
			{
			  unsigned char *snames = snames_of(parents);
			  *mess = gvl_vmess("[vq] %s: bad qualifier: %s is a form of %s", gp->text, qp->sign, snames);
			  free(snames);
			}
		    }
#endif
		}
	      /* Dont' free(b) because it belongs to wcs2utf */ 
	    }
	}
      free(tmp2);
    }
      
  if (!vp && !qp)
    *mess = gvl_vmess("[vq] unknown validation failure%s", QFIX);
      
  /* keep tmp until end because v and q give us the original text of
     the vq--the sign may have been canonicalized by gvl_validate */
  free(tmp);

  if (q_fixed)
    free(q_fixed);
  
  return ret;
}
