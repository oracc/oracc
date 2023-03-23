#include <stdlib.h>
#include <tree.h>

#include "sll.h"
#include "gvl.h"

/* This interface is used from a Node with name=g:q */
gvl_g*
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
  unsigned const char *l = NULL;
  g = sll_tmp_key(g,"qv");
  if ((l = gvl_lookup(g)))
    {
      gp->oid = (ccp)l;
      gp->sign = gvl_lookup(sll_tmp_key(l,""));
    }
  else
    {
      static unsigned char *mess = NULL;
      (void)gvl_vq_c10e(gp, &mess);
      if (mess)
	gp->mess = gvl_vmess("%s", mess);
      mess = NULL;
    }
}

int
gvl_vq_c10e(gvl_g *gp, unsigned char **mess)
{
  gvl_g *vp = NULL, *qp = NULL;
  unsigned const char *v, *q;
  unsigned char *tmp = malloc(strlen((ccp)gp->text)+1), *end = NULL;
  int pnest = 0;

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
    return -1;

  if (qp)
    {
      if (!qp->sign)
	q_bad = 1;
      else if (strcmp((ccp)qp->text, (ccp)qp->sign))
	{
	  q_fixed = malloc(strlen((ccp)qp->text) + strlen((ccp)qp->sign) + strlen(" [ <= ] "));
	  (void)sprintf((char*)q_fixed," [%s <= %s]", qp->sign, qp->text);
	}
    }
  else
    return -1;

  /* Now if we have bad value and qualifier it's too hard to guess */
  if (v_bad && q_bad)
    vq->mess = gvl_vmess("[vq]: value %s and qualifier %s both unknown%s", vp->text, qp->text, QFIX);
  else if (v_bad)
    {
      /* If the v is unknown, check if the base is known for q under a
	 different index, else report known v for q */
      unsigned char *altindex = sll_try_h(qp->oid, vp->text);
      if (!altindex)
	{
	  unsigned const char *tmp2 = gvl_lookup(sll_tmp_key((uccp)qp->oid, "values"));
	  if (tmp2)
	    vq->mess = gvl_vmess("[vq]: %s::%s unknown. Known for %s: %s%s", vp->text, qp->sign, qp->sign, tmp2, QFIX);
	  else
	    vq->mess = gvl_vmess("[vq]: %s::%s unknown. No known values for %s%s", vp->text, qp->sign, qp->sign, QFIX);
	}
      else
	; /* ought to be a 'x₂(Y) should be x₃(Y)' err here */
    }
  else if (q_bad)
    {
      /* If the q is unknown, report known q for v */
      unsigned const char *tmp2 = gvl_lookup(sll_tmp_key(vp->text, "q"));
      if (tmp2)
	vq->mess = gvl_vmess("[vq]: q %s unknown: known for %s: %s%s", qp->text, vp->text, tmp2, QFIX);
      else if (!strchr((ccp)qp->text,'X'))
	vq->mess = gvl_vmess("[vq]: q %s unknown: %s known as %s%s", qp->text, vp->text, vp->sign, QFIX);
    }
  else
    {
      unsigned char *tmp2 = malloc(strlen((ccp)vp->text) + strlen((ccp)qp->sign) + 3);
      sprintf((char*)tmp2, "%s(%s)", vp->text, qp->sign);
	  
      /* tmp2 is now a vq with valid v and q components */
      if (gvl_lookup(sll_tmp_key(tmp2,"qv")))
	{
	  extern int gvl_strict;
	  /* vq is known combo -- we have canonicalized the g that was passed as arg1 */
	  vq->oid = qp->oid;
	  vq->sign = gvl_lookup(sll_tmp_key((uccp)qp->oid,""));
	  /* add gp->text to g hash as key of tmp2 ? */
	  if (gvl_strict)
	    vq->mess = gvl_vmess("[vq] %s(%s): should be %s%s", vp->text, qp->text, tmp2, QFIX);
	  ret = 1;
	}
      else if ('s' == *vp->type || 'c' == *vp->type)
	{
	  /* This is a qualified uppercase value like TA@g(LAK654a) */
	  if (strcmp(vp->oid, qp->oid))
	    {
	      unsigned const char *parents = gvl_lookup(sll_tmp_key((uccp)qp->oid,"parents"));
	      if (parents)
		{
		  if (!strstr((ccp)parents, vp->oid))
		    {
		      unsigned char *snames = sll_snames_of(parents);
		      vq->mess = gvl_vmess("[vq] %s(%s): bad qualifier: %s is a form of %s%s",
					   vp->text, qp->text, qp->sign, snames, QFIX);
		      free(snames);
		    }
		}
	      else
		vq->mess = gvl_vmess("[vq] %s(%s): value and qualifier are different signs (%s vs %s)%s",
				     vp->text, qp->text, vp->sign, qp->sign, QFIX);
	    }
	}
      else
	{
	  /* is vq a v that doesn't need qualifying? */
	  if (vp->oid && qp->oid && !strcmp(vp->oid, qp->oid))
	    {
	      extern int gvl_strict;
	      vq->oid = qp->oid;
	      vq->sign = gvl_lookup(sll_tmp_key((uccp)qp->oid,""));
	      if (gvl_strict)
		vq->mess = gvl_vmess("[vq] %s(%s): unnecessary qualifier on value%s",
				     vp->text, qp->text, QFIX);
	      ret = 1; /* this is still OK--we have resolved the issue deterministically */
	    }
	  else
	    {
	      /* if the qv is unknown see if the value has the wrong index in the q context */
	      unsigned char *altindex = sll_try_h(qp->oid, vp->text);
	      if (!altindex)
		{
		  /* we know the q doesn't have a value which is correct except for the index;
		     report known q for v */
		  if ('v' == *vp->type)
		    {
		      unsigned const char *q_for_v = gvl_lookup(sll_tmp_key(vp->text, "q"));
		      if (q_for_v)
			vq->mess = gvl_vmess("[vq] %s(%s): unknown. Known for %s: %s%s",
					     vp->text, qp->text, vp->text, q_for_v, QFIX);
		      else
			vq->mess = gvl_vmess("[vq] %s(%s): %s is %s%s",
					     vp->text, qp->text, vp->text, vp->sign, QFIX);
		    }
#if 1
		  else
		    {
		      fprintf(stderr, "gvl: [vq] unhandled case for input %s(%s)\n",
			      vp->text, qp->text);
		    }
#else
		  else
		    {
		      unsigned const char *parents = gvl_lookup(sll_tmp_key((uccp)qp->oid,"parents"));
		      if (parents)
			{
			  unsigned char *snames = snames_of(parents);
			  vq->mess = gvl_vmess("[vq] %s(%s): bad qualifier: %s is a form of %s",
					       vp->text, qp->text, qp->sign, snames);
			  free(snames);
			}
		    }
#endif
		}
	      else
		; /* port old should be x₂ should be x₃ mess here */
	      /* Dont' free(b) because it belongs to wcs2utf */ 
	    }
	}
      free(tmp2);
    }
      
  if (!vp && !qp)
    vq->mess = gvl_vmess("[vq] unknown validation failure%s", QFIX);
      
  if (q_fixed)
    free(q_fixed);
  
  return ret;
}
