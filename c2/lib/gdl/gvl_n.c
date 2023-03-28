#include <stdlib.h>
#include <tree.h>
#include <oraccsys.h>

#include "sll.h"
#include "gvl.h"

void
gvl_n(Node *ynp)
{
  gvl_g *nq = NULL;
  unsigned const char *l = NULL;
  unsigned char *p = (ucp)pool_alloc(strlen(ynp->kids->text) + strlen(ynp->kids->next->text) + 3, curr_sl->p);
  sprintf((char*)p, "%s(%s)", ynp->kids->text, ynp->kids->next->text);

  if (!(nq = hash_find(curr_sl->h, p)))
    {
      nq = memo_new(curr_sl->m);
      nq->type = "n";
      ynp->name = "g:n";
      nq->c10e = nq->orig = (uccp)p; /* no c10e for numbers yet */
      ynp->kids->name = "g:r";
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
  ynp->user = nq;
}
