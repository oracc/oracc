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

  if (p && !(vq = hash_find(curr_sl->h,p)))
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
	    {
	      vs = (ccp)((gvl_g*)(ynp->kids->user))->c10e;
	      if (!vs)
		vs = (ccp)((gvl_g*)(ynp->kids->user))->orig;
	    }
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
  if (vq->orig)
    {
      ynp->text = (ccp)vq->orig;
      gdl_prop_kv(ynp, GP_ATTRIBUTE, PG_GDL_INFO, "form", (ccp)vq->orig);
    }
}

static void
gvl_oid_add(Hash *u, const char *oids, const char *except)
{
  char buf[strlen(oids)+1], *tok, *ptr = buf;
  strcpy(buf, oids);
  while ((tok = strtok(ptr," ")))
    {
      if (!except || strcmp(tok, except))
	{
	  if (!hash_find(u,(uccp)tok))
	    {
	      /* The OID should be in the signlist hash so we can
		 reference the key there rather than allocating a new
		 instance */
	      const unsigned char *oidkey = hash_exists(curr_sl->sl, (uccp)tok);
	      if (!oidkey)
		oidkey = pool_copy((uccp)tok, curr_sl->p);
	      hash_add(u,(uccp)oidkey,"");
	    }
	}
      if (ptr)
	ptr = NULL;
    }
}

/* Find all the OIDs that this sign can be a proxy for */
static const char *
gvl_oid_set(gvl_g *gp)
{
  Hash *u = hash_create(7);
  const char *s, **k;
  char *ret;
  int nkeys;

  /* If gp is a pseudo-signname then c10e will have left oids in c10e */
  if (gp->c10e && 'o' == *gp->c10e)
    gvl_oid_add(u, (ccp)gp->c10e, NULL);

  /* if gp is a LISTNUM then ';l' will have one or more signs it can refer to */
  s = (ccp)gvl_lookup(sll_tmp_key(gp->orig,"l"));
  if (s && *s)
    gvl_oid_add(u, s, gp->oid);

  /* if gp is a FORM then ';signs' will have one or more signs that are parents */
  s = (ccp)gvl_lookup(sll_tmp_key((uccp)gp->oid,"signs"));
  if (s && *s)
    gvl_oid_add(u, s, NULL);

  /* if gp is a SIGN then ';forms' will have one or more forms that are alternatives for it */
  s = (ccp)gvl_lookup(sll_tmp_key((uccp)gp->oid,"forms"));
  if (s && *s)
    {
      gvl_oid_add(u, s, NULL);
      /* We need self->oid in the set as well when the gp is a sign, i.e., it has forms */
      if (gp->oid)
	gvl_oid_add(u, gp->oid, NULL);
    }
  
  k = hash_keys2(u,&nkeys);
  ret = (char*)pool_alloc(nkeys*strlen("o1234567 "), curr_sl->p);
  *ret = '\0';
  int i;
  for (i = 0; i < nkeys; ++i)
    {
      if (i)
	strcat(ret, " ");
      strcat(ret, k[i]);
    }
  hash_free(u, NULL);
  return ret;
}

static const char *
gvl_common_oids(const char *o1, const char *o2)
{
  List *c = list_create(LIST_SINGLE);
  const char *target, *test, *ret = NULL;
  char *srch;
  if (strlen(o1) > strlen(o2))
    {
      target = o1;
      srch = strdup(o2);
    }
  else
    {
      target = o2;
      srch = strdup(o1);
    }
  while ((test = strtok(srch, " ")))
    {
      if (strstr(target, test))
	list_add(c, (void*)test);
      if (srch)
	srch = NULL;
    }
  free(srch);

  if (list_len(c))
    ret = (ccp)list_join(c, " ");
  list_free(c, NULL);

  return ret;  
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
	    vq->mess = gvl_vmess("[sb1] %s: should be %s%s", vq->orig, qp->sign, QFIX);
	  else if (strcmp((ccp)vp->orig,(ccp)altindex) || (ccp)q_fixed)
	    vq->mess = gvl_vmess("[sb2] %s: should be %s(%s)%s", vq->orig, altindex, qp->sign, QFIX);
	}
    }
  else if (q_bad)
    {
      if (!gdl_corrq && !gvl_void_messages)
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
	    vq->mess = gvl_vmess("[sb3] %s(%s): should be %s%s", vp->orig, qp->orig, tmp2, QFIX);
	  ret = 1;
	}
      else if ('s' == *vp->type || 'c' == *vp->type)
	{
	  /* This is a qualified uppercase value like TA@g(LAK654a) */
	  if (vp->oid && qp->oid)
	    {
	      if (strcmp(vp->oid, qp->oid))
		{
		  /* There could be ambiguity in either or both of vp and
		   * qp because if either is a LISTNUM that could resolve
		   * to multiple OIDs.  Similarly, if either resolves to a
		   * value that has multiple forms, like ŠUŠₓ, that, too,
		   * will have multiple OIDs.
		   *
		   * The solution is to find any-possible-OID lists for
		   * the two parts and then see if they have a single OID
		   * in common; if so, it's a legal match-up; if they have
		   * multiple OIDs in common or no common OIDs, it's an
		   * error.
		   */
		  const char *voids = NULL, *qoids = NULL, *common;
		  voids = gvl_oid_set(vp);
		  qoids = gvl_oid_set(qp);
		  common = gvl_common_oids(voids, qoids);
			
		  if (!common)
		    {
		      vq->mess = gvl_vmess("[vq] %s(%s): value and qualifier are different signs (%s vs %s)%s",
					   vp->orig, qp->orig, vp->sign, qp->sign, QFIX);
		    }
		  else if (strchr(common, ' '))
		    {
		      /* This is strange but can happen with circular definitions like:
		       *
		       * @sign GURUŠ
		       * @form KAL
		       * ...
		       * @sign KAL
		       * @form GURUŠ
		       * ...
		       *
		       * If the OID for V is in the OIDs for Q (i.e.,
		       * qoids) we use that for the qv sign/oid,
		       * assuming that:
		       *
		       * GURUŠ(KAL)
		       *
		       * is supposed to mean "this is an instance of
		       * the GURUŠ sign written with the KAL form".
		       *
		       * Hence, the qv as whole will have the sign
		       * identity of the V rather than the Q, contrary
		       * to normal practice.
		       *
		       */
		      if (strstr(qoids, vp->oid))
			{
			  vq->oid = vp->oid;
			  vq->sign = vp->sign;
			}
		      else
			{
			  unsigned char *snames = sll_snames_of((uccp)common);
			  vq->mess = gvl_vmess("[vq] %s(%s): could mean multiple possible signs: %s%s",
					       vp->orig, qp->orig, snames, QFIX);
			  free(snames);
			}
		    }
		  else
		    {
		      /* We have a single legal OID--reset vp/qp/vq appropriately */
		      const unsigned char *sign = gvl_lookup((uccp)common);
		      if (vp->c10e && 'o' == *vp->c10e)
			vp->c10e = vp->orig;
		      vp->sign = qp->sign = vq->sign = sign;
		      vp->oid = qp->oid = vq->oid = common;
		    }
		}
	      else
		{
		  /* vp and qp have same OID which is correct */
		}
	    }
	  else
	    {
	      /* vp or qp has NULL OID: "this can't happen" */
	      abort();
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
		  /* see if the q is known by other OIDs--this can
		     happen if a LISTNUM is both a sign-name and a
		     @list entry somewhere */
		  const char *altoid = (ccp)gvl_lookup(sll_tmp_key(qp->orig, "l"));
		  const char *newoid;
		  if (altoid && vp->oid && (newoid = strstr(altoid,vp->oid)))
		    {
		      char *s = (char*)qp->oid;
		      while (*newoid)
			*s++ = *newoid++;
		    }
		  else
		    {
		      /* we know the q doesn't have a value which is
			 correct except for the index and we can't find
			 another OID for the q; report known q for v */
		      if ('v' == *vp->type)
			{
			  if (!gvl_void_messages)
			    {
			      unsigned const char *q_for_v = gvl_lookup(sll_tmp_key(vp->orig, "q"));
			      if (q_for_v)
				vq->mess = gvl_vmess("[vq] %s(%s): unknown. Known for %s: %s%s",
						     vp->orig, qp->orig, vp->orig, q_for_v, QFIX);
			      else
				vq->mess = gvl_vmess("[vq] %s(%s): %s is %s not %s%s",
						     vp->orig, qp->orig, vp->orig, vp->sign, qp->orig, QFIX);
			    }
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
			  if (!gdl_corrq && ('n' != *vp->orig && 'N' != *vp->orig))
			    vq->mess = gvl_vmess("[vq] %s(%s): mismatched number qualifier",
						 vp->orig, qp->orig);
			}
		      else
			{
			  mesg_vwarning(currgdlfile, gdllineno, "gvl: [vq] unhandled case for input %s(%s)\n",
					vp->orig, qp->orig);
			}
		    }
		}
	      else
		{
		  if (gvl_v_isupper(vp->orig) && !strcmp((ccp)(altindex=g_uc(altindex)), (ccp)qp->sign))
		    vq->mess = gvl_vmess("[sb4] %s: should be %s%s", vq->orig, qp->sign, QFIX);
		  else if (strcmp((ccp)vp->orig,(ccp)altindex))
			vq->mess = gvl_vmess("[sb5] %s: should be %s(%s)%s", vq->orig, altindex, qp->sign, QFIX);
		  else if ((ccp)q_fixed)
		    {
		      if (gvl_strict)
			vq->mess = gvl_vmess("[sb6] %s: should be %s(%s)%s", vq->orig, altindex, qp->sign, QFIX);
		    }
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
