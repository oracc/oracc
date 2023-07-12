#include <stdlib.h>
#include <tree.h>
#include <oraccsys.h>

#include "sll.h"
#include "gdl.h"
#include "gvl.h"

void
gvl_n(Node *ynp)
{
  Node *nnp = ynp;
  gvl_g *nq = NULL;
  unsigned const char *l = NULL;
  unsigned char *p = NULL;

  if (!nnp->kids->text && nnp->kids->next->text)
    nnp = nnp->kids;
  
  p = (ucp)pool_alloc(strlen(nnp->kids->text) + strlen(nnp->kids->next->text) + 3, curr_sl->p);
  sprintf((char*)p, "%s(%s)", nnp->kids->text, nnp->kids->next->text);

  /* make sure these are all fixed even if the num is in the hash */
  ynp->name = "g:n";
  ynp->kids->name = "g:r";
  ynp->kids->next->name = ((sll_has_sign_indicator((uccp)ynp->kids->next->text) ? "g:s" : "g:v"));

  if (!(nq = hash_find(curr_sl->h, p)))
    {
      nq = memo_new(curr_sl->m);
      nq->type = "n";
      nq->c10e = nq->orig = (uccp)p; /* no c10e for numbers yet */
      hash_add(curr_sl->h, nq->orig, nq);
      if ((l = gvl_lookup(p)))
	{
	  nq->oid = (ccp)l;
	  nq->sign = gvl_lookup(sll_tmp_key(l,""));
	}
      else
	{
	  const unsigned char *lg = utf_lcase(p);
	  if ((l = gvl_lookup(lg)))
	    {
	      nq->oid = (ccp)l;
	      nq->sign = gvl_lookup(sll_tmp_key(l,""));
	    }
	  else if ('n' == *lg)
	    {
	      char *oneify = strdup((ccp)lg);
	      *oneify = '1';
	      if ((l = gvl_lookup((uccp)oneify)))
		{
		  nq->oid = (ccp)l;
		  nq->sign = gvl_lookup(sll_tmp_key(l,""));
		}
	      else
		nq->mess = gvl_vmess("expected to validate %s via %s but %s doesn't exist", p, oneify, oneify);
	      if (oneify)
		free(oneify);
	    }
	  else
	    {
	      nq->mess = gvl_vmess("unknown numeric sign %s", p);
	    }
	}
    }
  if (gdl_orig_mode)
    ynp->text = (ccp)nq->orig;
  else
    ynp->text = (ccp)nq->c10e;
  ynp->user = nq;
}

void
gvl_n_sexify(Node *ynp)
{
  gvl_g *nq = NULL;
  unsigned char *p = (ucp)pool_alloc(strlen(ynp->text) + strlen("diš") + 3, curr_sl->p);
  sprintf((char*)p, "%s(%s)", ynp->text, "diš");

  if (!(nq = hash_find(curr_sl->h, p)))
    {
      Tree *ntp = tree_init();
      Node *top = NULL;
      int sexnum = atoi(ynp->text);

      top = tree_root(ntp, NS_GDL, "g:gp", 1, ynp->mloc);

      if (sexnum > 0)
	{
	  char *lnp = NULL;
	  /* sexify the decimal number */
	  unsigned char *snum = sexify(sexnum, "d");
	  
	  /* split it into a list if it is, e.g., 1(u) 2(diš) */
	  List *sl = list_from_str((char *)snum, NULL, LIST_SINGLE);

	  /* create a q-node for each element in the list */
	  for (lnp = list_first(sl); lnp; lnp = list_next(sl))
	    {
	      char *n = NULL, *q = NULL;
	      char *tmp = strdup(lnp);
	      Node *qnp = NULL;
	      n = q = tmp;
	      while (*q && '(' != *q)
		++q;
	      if ('(' == *q)
		{
		  char *qt = NULL;
		  *q++ = '\0';
		  qt = q;
		  while (*qt && ')' != *qt)
		    ++qt;
		  if (*qt)
		    *qt = '\0';
		}
	      else
		mesg_err(ynp->mloc, "internal error: sexify returned number without parens");	      
	      gdl_push(ntp, "g:q");
	      qnp = gdl_number(ntp, (ccp)pool_copy((uccp)n,ynp->tree->tm->pool));
	      qnp = qnp->rent;

	      if (*q)
		gdl_graph(ntp,(ccp)pool_copy((uccp)q,ynp->tree->tm->pool));
	      
	      /* process each q-node as though it were a regular qualified number */
	      gvl_n(qnp);
	      gdl_pop(ntp, "g:q");
	      free(tmp);
	    }
	  
	  /* on the singleton or group set a property that this has been
	     sexified and record the original number in the text field */
	  gdl_prop(top, GP_IMPLICIT, PG_GDL_INFO);
	  top->text = (void*)ynp->text;

	  /* transfer any other properties from the original node to
	     the g:gp; this ensures that in '1!' the '!' is raised to
	     the top level */
	  prop_merge(top->props, ynp->props);
	  
	  /* update the ynp data with the sexified data */
	  node_replace(top, ynp);
	}
      else if (!strcmp(ynp->text, "00"))
	{
	  ynp->name = "g:n";
	  /* for now leave this as a node with no gvl_g because 00
	     isn't a grapheme but it is a number */
	}      
      else
	/* n/N */;
    }
}
