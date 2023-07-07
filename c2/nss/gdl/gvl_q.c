#include <stdlib.h>
#include <tree.h>
#include <unidef.h>
#include <gutil.h>

#include "sll.h"
#include "gdl.h"
#include "gvl.h"

extern const char *currgdlfile;
extern int gdllineno;
extern int gdl_corrq;

void
gvl_q(Node *ynp)
{
  gvl_g *vq = NULL;
  unsigned char *o = NULL, *p = NULL;
  unsigned const char *vo = NULL, *qo = NULL;

  gdl_corrq = (prop_find_pg(ynp->kids->props,'!',PG_GDL_FLAGS)!=NULL);

  if (gdl_corrq)
    {
      ynp->name = "g:corr";
      /* the child nodes have been processed already, so we
	 just don't build a gvl_g node for a corr */
      return;
    }

  /* This builds a c10e version of vq */
  if (!(vo = (uccp)ynp->kids->text))
    if (ynp->kids->user)
      {
	if (!(vo = ((gvl_g*)(ynp->kids->user))->c10e))
	  vo = ((gvl_g*)(ynp->kids->user))->orig;
      }
  if (!(qo = (uccp)ynp->kids->next->text))
    if (ynp->kids->next->user)
      {
	if (!(qo = ((gvl_g*)(ynp->kids->next->user))->c10e))
	  qo = ((gvl_g*)(ynp->kids->next->user))->orig;
      }  

  if (vo && qo)
    {
      p = (ucp)pool_alloc(strlen((ccp)vo) + strlen((ccp)qo) + 3, curr_sl->p);
      sprintf((char*)p, "%s(%s)", (ccp)vo, (ccp)qo);
    }

  if (p && !(vq = hash_find(curr_sl->h, p)))
    {
      vq = memo_new(curr_sl->m);
      vq->type = "q";

      /* This builds an orig version of vq */
      if (ynp->kids->user)
	vo = ((gvl_g*)(ynp->kids->user))->orig;
      else
	vo = (uccp)ynp->kids->text;
      if (ynp->kids->next->user)
	qo = ((gvl_g*)(ynp->kids->next->user))->orig;
      else
	qo = (uccp)ynp->kids->next->text;
      if (vo && qo)
	{
	  o = (ucp)pool_alloc(strlen((ccp)vo) + strlen((ccp)qo) + 3, curr_sl->p);
	  sprintf((char*)o, "%s(%s)", (ccp)vo, (ccp)qo);
	}
      vq->orig = (uccp)o;

      if (gvl_q_c10e(ynp->kids->user, ynp->kids->next->user, vq) > 0)
	{
	  const char *vs = NULL;
	  const char *qs = NULL;
	  if (ynp->kids->user)
	    vs = (ccp)((gvl_g*)(ynp->kids->user))->c10e;
	  else
	    vs = ynp->kids->text;
	  if (ynp->kids->next->user)
	    qs = (ccp)((gvl_g*)(ynp->kids->next->user))->sign;
	  else
	    qs = ynp->kids->next->text;
	  
	  if (!vs)
	    vs = ynp->kids->text;
	  if (!qs)
	    {
	      /* This could be q node used as a nested q, e.g., kisimₓ(|DAG.KISIM₅×LU|(LAK721)) */
	      if ('q' == ynp->kids->next->name[2])
		qs = ynp->kids->next->kids->text;
	      else
		qs = ynp->kids->next->text;
	    }

	  p = (ucp)pool_alloc(strlen(vs) + strlen(qs) + 3, curr_sl->p);
	  sprintf((char*)p, "%s(%s)", vs, qs);
	  vq->c10e = (uccp)p;
	}
      else
	{
	  vq->orig = vo ? vo : qo;
	  vq->mess = gvl_vmess("%s failed canonicalization", vq->orig);
	}

      if (vq->orig && vq->c10e) /* don't hash if failed canonicalization */
	{
	  hash_add(curr_sl->h, vq->orig, vq);
	  if (vq->c10e && strcmp((ccp)vq->orig, (ccp)vq->c10e))
	    hash_add(curr_sl->h, vq->c10e, vq);
	}
    }
  else if (!p)
    {
      vq = memo_new(curr_sl->m);
      vq->orig = (uccp)ynp->text;
      vq->mess = gvl_vmess("%s unable to attempt canonicalization", ynp->text);
    }
  ynp->user = vq;
}

int
gvl_q_c10e(gvl_g *vp, gvl_g *qp, gvl_g *vq)
{
  int v_bad = 0, q_bad = 0, ret = 1;
  unsigned char *q_fixed = NULL;

  if (vp)
    {
      if (vp->mess && !strstr((ccp)vp->mess, "must be qualified"))
	v_bad = 1;
      else if (!vp->oid && !strstr((ccp)vp->orig, U_s_x_u8str))
	v_bad = 1;
    }
  else
    return -1;

  if (qp)
    {
      if (!qp->sign || !qp->oid)
	q_bad = 1;
      else if (strcmp((ccp)qp->orig, (ccp)qp->sign))
	{
	  q_fixed = malloc(strlen((ccp)qp->orig) + strlen((ccp)qp->sign) + strlen(" [ <= ] "));
	  (void)sprintf((char*)q_fixed," [%s <= %s]", qp->sign, qp->orig);
	}
    }
  else
    return -1;

  /* Now if we have bad value and qualifier it's too hard to guess */
  if (v_bad && q_bad)
    vq->mess = gvl_vmess("[vq]: value %s and qualifier %s both unknown%s", vp->orig, qp->orig, QFIX);
  else if (v_bad)
    {
      /* If the v is unknown, check if the base is known for q under a
	 different index, else report known v for q */
      unsigned char *altindex = sll_try_h(qp->oid, vp->orig);
      if (!altindex)
	{
	  unsigned const char *tmp2 = gvl_lookup(sll_tmp_key((uccp)qp->oid, "values"));
	  if (tmp2)
	    vq->mess = gvl_vmess("[vq]: %s::%s unknown. Known for %s: %s%s", vp->orig, qp->sign, qp->sign, tmp2, QFIX);
	  else
	    vq->mess = gvl_vmess("[vq]: %s::%s unknown. No known values for %s%s", vp->orig, qp->sign, qp->sign, QFIX);
	}
      else
	{
	  /* If the gp->text value is uppercase, make the result value
	     uppercase; then if value == qp->sign, elide the value and
	     just print the qp-sign with no parens [FIRST ERROR MESSAGE MAY BE IN WRONG PLACE] */
	  if (gvl_v_isupper(vp->orig) && !strcmp((ccp)(altindex=g_uc(altindex)), (ccp)qp->sign))
	    vq->mess = gvl_vmess("%s: should be %s%s", vq->orig, qp->sign, QFIX);
	  else if (strcmp((ccp)vp->orig,(ccp)altindex) || (ccp)q_fixed)
	    vq->mess = gvl_vmess("%s: should be %s(%s)%s", vq->orig, altindex, qp->sign, QFIX);
	}
    }
  else if (q_bad)
    {
      if (!gdl_corrq)
	{
	  /* If the q is unknown, report known q for v */
	  unsigned const char *tmp2 = gvl_lookup(sll_tmp_key(vp->orig, "q"));
	  if (tmp2)
	    vq->mess = gvl_vmess("[vq]: q %s unknown: known for %s: %s%s", qp->orig, vp->orig, tmp2, QFIX);
	  else if (!strchr((ccp)qp->orig,'X'))
	    vq->mess = gvl_vmess("[vq]: q %s unknown: %s known as %s%s", qp->orig, vp->orig, vp->sign, QFIX);
	}
      else
	gdl_corrq = 0;
    }
  else
    {
      unsigned char *tmp2 = malloc(strlen((ccp)vp->orig) + strlen((ccp)qp->sign) + 3);
      sprintf((char*)tmp2, "%s(%s)", vp->orig, qp->sign);
	  
      /* tmp2 is now a vq with valid v and q components */
      if (gvl_lookup(sll_tmp_key(tmp2,"qv")))
	{
	  extern int gvl_strict;
	  /* vq is known combo -- we have canonicalized the g that was passed as arg1 */
	  vq->oid = qp->oid;
	  vq->sign = gvl_lookup(sll_tmp_key((uccp)qp->oid,""));
	  /* add gp->orig to g hash as key of tmp2 ? */
	  if (gvl_strict && strcmp((ccp)vq->orig, (ccp)tmp2))
	    vq->mess = gvl_vmess("[vq] %s(%s): should be %s%s", vp->orig, qp->orig, tmp2, QFIX);
	  ret = 1;
	}
      else if ('s' == *vp->type || 'c' == *vp->type)
	{
	  /* This is a qualified uppercase value like TA@g(LAK654a) */
	  if (vp->oid && qp->oid && strcmp(vp->oid, qp->oid))
	    {
	      unsigned const char *parents = gvl_lookup(sll_tmp_key((uccp)qp->oid,"parents"));
	      if (parents)
		{
		  if (!strstr((ccp)parents, vp->oid))
		    {
		      unsigned char *snames = sll_snames_of(parents);
		      vq->mess = gvl_vmess("[vq] %s(%s): bad qualifier: %s is a form of %s%s",
					   vp->orig, qp->orig, qp->sign, snames, QFIX);
		      free(snames);
		    }
		}
	      else
		vq->mess = gvl_vmess("[vq] %s(%s): value and qualifier are different signs (%s vs %s)%s",
				     vp->orig, qp->orig, vp->sign, qp->sign, QFIX);
	    }
	  else
	    {
	      if (qp->oid)
		{
		  vq->oid = qp->oid;
		  vq->sign = qp->sign;
		}
	      else
		{
		  vq->mess = gvl_vmess("[vq] %s(%s): failed to set OID on qualified sign%s",
				       vp->orig, qp->orig, QFIX);
		}
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
				     vp->orig, qp->orig, QFIX);
	      ret = 1; /* this is still OK--we have resolved the issue deterministically */
	    }
	  else
	    {
	      /* if the qv is unknown see if the value has the wrong index in the q context */
	      unsigned char *altindex = sll_try_h(qp->oid, vp->orig);
	      if (!altindex)
		{
		  /* we know the q doesn't have a value which is correct except for the index;
		     report known q for v */
		  if ('v' == *vp->type)
		    {
		      unsigned const char *q_for_v = gvl_lookup(sll_tmp_key(vp->orig, "q"));
		      if (q_for_v)
			vq->mess = gvl_vmess("[vq] %s(%s): unknown. Known for %s: %s%s",
					     vp->orig, qp->orig, vp->orig, q_for_v, QFIX);
		      else
			vq->mess = gvl_vmess("[vq] %s(%s): %s is %s%s",
					     vp->orig, qp->orig, vp->orig, vp->sign, QFIX);
		    }
		  else if ('p' == *vp->type)
		    {
		      /* We retain a quirk of the original ATF
			 specification which is that '*' is a wildcard
			 punctuation; it defaults to DIŠ, but if
			 qualified then the qualifier takes
			 precedence.  No error here because it's
			 allowable for v and q to be different
			 signs */
		      if ('*' != *vp->orig)
			vq->mess = gvl_vmess("[vq] %s(%s): mismatched punctuation qualifier",
					     vp->orig, qp->orig);
		    }
		  else if ('n' == *vp->type)
		    {
		      if (!gdl_corrq)
			vq->mess = gvl_vmess("[vq] %s(%s): mismatched number qualifier",
					     vp->orig, qp->orig);
		    }
#if 1
		  else
		    {
		      mesg_vwarning(currgdlfile, gdllineno, "gvl: [vq] unhandled case for input %s(%s)\n",
			      vp->orig, qp->orig);
		    }
#else
		  else
		    {
		      unsigned const char *parents = gvl_lookup(sll_tmp_key((uccp)qp->oid,"parents"));
		      if (parents)
			{
			  unsigned char *snames = snames_of(parents);
			  vq->mess = gvl_vmess("[vq] %s(%s): bad qualifier: %s is a form of %s",
					       vp->orig, qp->orig, qp->sign, snames);
			  free(snames);
			}
		    }
#endif
		}
	      else
		{
		  if (gvl_v_isupper(vp->orig) && !strcmp((ccp)(altindex=g_uc(altindex)), (ccp)qp->sign))
		    vq->mess = gvl_vmess("%s: should be %s%s", vq->orig, qp->sign, QFIX);
		  else if (strcmp((ccp)vp->orig,(ccp)altindex) || (ccp)q_fixed)
		    vq->mess = gvl_vmess("%s: should be %s(%s)%s", vq->orig, altindex, qp->sign, QFIX);
		}
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

#if 0
static int gvl_q_c10e(gvl_g *gp, unsigned char **mess);
int
gvl_q_c10e(gvl_g *gp, unsigned char **mess)
{
  gvl_g *vp = NULL, *qp = NULL, *vq = NULL;
  char *p = NULL;
  unsigned const char *v, *q;
  unsigned char *tmp = malloc(strlen((ccp)gp->orig)+1), *end = NULL;
  int pnest = 0;

  /* get pointers to the value (v) and qualifier (q) parts */
  strcpy((char*)tmp, (ccp)gp->orig);
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
      vq->mess = gvl_vmess("[vq] %s: syntax error in value-qualifier", gp->orig);
      return 0;
    }
  
  q = end--;
  *end = '\0';
  v = tmp;

  /* check the value */
  vp = gvl_validate(v);
      
  /* check the sign */
  qp = gvl_validate(q);

  vq = memo_new(curr_sl->m);
  p = (char*)pool_alloc(strlen((ccp)vp->orig) + strlen((ccp)qp->orig) + 3, curr_sl->p);
  sprintf(p, "%s(%s)", vp->orig, qp->orig);
  vq->orig = (uccp)p;
  if (gvl_q(vp, qp, vq))
    {
      p = (char*)pool_alloc(strlen((ccp)vp->c10e) + strlen((ccp)qp->sign) + 3, curr_sl->p);
      sprintf(p, "%s(%s)", vp->c10e, qp->sign);
      vq->c10e = (uccp)p;
      return 1;
    }

  return 0;
}
#endif

#if 0
/* This interface is used from a Node with name=g:q */
gvl_g*
gvl_q_node(Node *np)
{
  gvl_g *qv = NULL;
  gvl_vq_gg(np->kids->user, np->kids->next->user, qv);
  return qv;
}
#endif

#if 0
/* This interface is used when gvl_validate is passed a vq, e.g., e₄(A) */
void
gvl_q_bis(unsigned const char *g, gvl_g *gp)
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
#endif

