#include <string.h>
#include <psd_base.h>
#include "nsa.h"

static int
scmp(struct nsa_step **a, struct nsa_step**b)
{
  return strcmp((*a)->sys->n,(*b)->sys->n);
}

static struct nsa_token *
collect_measure(struct nsa_token *first, struct nsa_parser *p)
{
  List *mtoks = list_create(LIST_SINGLE);
  struct nsa_token *t;
  struct nsa_token *m;
  Hash_table *cands = hash_create(10);
  int i;

  for (t = first; t; t = list_next(p->toks))
    {
      if (t->type == NSA_T_QUANTITY)
	{
	  struct nsa_step *s;
	  if (cands->key_count)
	    {
	      Hash_table *newcands = hash_create(10);
	      struct nsa_step *found_step;
	      for (s = list_first(quantity_cands(t)); 
		   s; 
		   s = list_next(quantity_cands(t)))
		if ((found_step = hash_find(cands, (unsigned char *)s->sys->n))
		    && found_step->aev->num >= s->aev->num)
		  hash_add(newcands, (unsigned char *)s->sys->n, s);
	      if (newcands->key_count)
		{
		  hash_free(cands, NULL);
		  cands = newcands;
		  list_add(mtoks,t);
		}
	      else
		{
		  (void)list_unnext(p->toks);
		  break;
		}
	    }
	  else
	    {
	      for (s = list_first(quantity_cands(t)); 
		   s; 
		   s = list_next(quantity_cands(t)))
		hash_add(cands, (unsigned char *)s->sys->n, s);
	      list_add(mtoks,t);
	    }
	}
      else
	{
	  (void)list_unnext(p->toks);
	  break;
	}
    }

  m = new_token();
  m->type = NSA_T_MEASURE;
  m->children = new_children(list_len(mtoks));
  for (i = 0, t = list_first(mtoks); t; t = list_next(mtoks), ++i)
    m->children[i] = t;
  list_free(mtoks,NULL);
  m->d.m = new_measure();
  measure_cands(m) = hash2list(cands, (sort_cmp_func*)scmp);
  hash_free(cands,NULL);
  if (list_len(measure_cands(m)) == 1)
    {
      measure_system(m) = ((struct nsa_step*)list_first(measure_cands(m)))->sys;
      list_free(measure_cands(m),NULL);
      measure_cands(m) = NULL;
    }
  else if (list_len(measure_cands(m)) == 0)
    {
      measure_system(m) = NULL;
      list_free(measure_cands(m),NULL);
      measure_cands(m) = NULL;
    }
  return m;
}

void
nsa_measure(struct nsa_parser *p)
{
  List *newtoks = list_create(LIST_DOUBLE);
  struct nsa_token *t;
  for (t = list_first(p->toks); t; t = list_next(p->toks))
    {
      if (t->type == NSA_T_QUANTITY)
	list_add(newtoks,collect_measure(t, p));
      else
	list_add(newtoks,t);
    }
  list_free(p->toks,NULL);
  p->toks = newtoks;
}

void
nsa_measure_wrapup(struct nsa_token *m, struct nsa_parser *p)
{
  struct nsa_token *q;
  struct nsa_count *m_total = new_count();
  int i = 0;
  m_total->den = 1;
  while ((q = m->children[i++]))
    {
      struct nsa_step *s;
      for (s = list_first(unit_cands(quantity_unit(q))); 
	   s; 
	   s = list_next(unit_cands(quantity_unit(q))))
	{
	  if (!strcmp(s->sys->n,measure_system(m)->n))
	    {
	      unit_step(quantity_unit(q)) = s;
	      quantity_aev(q) = new_count();
	      quantity_aev(q)->num = count_num(quantity_count(q)) * s->aev->num;
	      /* step aev denominator is always 1 */
	      quantity_aev(q)->den = count_den(quantity_count(q));
	      nsa_add_frac(m_total,quantity_aev(q));
	      break;
	    }
	}
      /* Don't free this one; it gets freed in h_del_hash_data */
      /*list_free(unit_cands(quantity_unit(q)), NULL);*/
      unit_cands(quantity_unit(q)) = NULL;
    }
  measure_aev(m) = m_total;
  measure_mev(m) = new_count();
  *(measure_mev(m)) = *(measure_system(m)->mev);
  nsa_mul_frac(measure_mev(m),measure_aev(m));
  measure_rendered_mev(m) = nsa_modern(measure_mev(m),measure_system(m)->meu);
}
