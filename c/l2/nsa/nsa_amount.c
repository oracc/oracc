#include "nsa.h"

static struct nsa_token *
collect_amount(struct nsa_token *measure, struct nsa_parser *p)
{
  struct nsa_token *amount = new_token();
  struct nsa_token *t;
  int found_sysdet = 0;
  int really_count = 0;
  if (measure->type == NSA_T_COUNT)
    really_count = 1;
  amount->type = NSA_T_AMOUNT;
  amount->d.a = new_amount();
  amount_measure(amount) = measure;
  t = list_next(p->toks);
  if (t && t->type == NSA_T_GRAPHEME)
    {
      struct nsa_system *sysp = NULL;
      if (!really_count && (sysp = hash_find(p->context->sysdets,nsa_grapheme_text(t))))
	{
	  measure_sysdet(measure) = t;
	  grapheme_is_sysdet(t) = sysp;
	  found_sysdet = 1;
	}
      else
	{
	  int ntries = 4;
	  List *comtoks = list_create(LIST_SINGLE);
	  List_node *np = p->toks->rover;
	  struct nsa_token *c = new_token();
	  int i;
	  c->type = NSA_T_COMMODITY;
	  c->d.o = new_commodity();
	  amount_commodity(amount) = c;
	  list_add(comtoks,t);
	  commodity_head(c) = (const char *)nsa_grapheme_text(t);
	  commodity_cands(c) = hash_find(p->context->comheads,
					 (unsigned char*)commodity_head(c));
	  while (ntries-- > 0)
	    {
	      if (np->next)
		{
		  np = np->next;
		  t = np->data;
		  if (t->type == NSA_T_GRAPHEME)
		    {
		      struct nsa_system *sysp;
		      if (!really_count && (sysp = hash_find(p->context->sysdets,nsa_grapheme_text(t))))
			{
			  measure_sysdet(measure) = t;
			  grapheme_is_sysdet(t) = sysp;
			  break;
			}
		      else
			list_add(comtoks,t);
		    }
		  else
		    ntries = 0;
		}
	    }

	  if (list_len(comtoks))
	    {
	      if (found_sysdet)
		{
		  c->children = new_children(list_len(comtoks));
		  for (i = 0, t = list_first(comtoks); t; t = list_next(comtoks), ++i)
		    {
		      c->children[i] = t;
		      if (i)
			(void)list_next(p->toks);
		    }
		}
	      else
		{
		  /* Only take the head unless we found a system determinative */
		  c->children = new_children(1);
		  c->children[0] = list_first(comtoks);
		}
	    }
	  /* else reject the commodity--only accept ones which are registered */
	  else
	    {
	      (void)list_unnext(p->toks);
	      nsa_del_token(c);
	      amount_commodity(amount) = NULL;
	    }
	  list_free(comtoks,NULL);
	}
    }
  /* may have to else-if a sze_trap here */
  else if (t)
    (void)list_unnext(p->toks);
  return amount;
}

void
nsa_amount(struct nsa_parser *p)
{
  List *newtoks = list_create(LIST_DOUBLE);
  struct nsa_token *t;
  for (t = list_first(p->toks); t; t = list_next(p->toks))
    {
      if (t->type == NSA_T_MEASURE || t->type == NSA_T_COUNT)
	list_add(newtoks,collect_amount(t,p));
      else
	list_add(newtoks,t);
    }
  list_free(p->toks,NULL);
  p->toks = newtoks;
}
