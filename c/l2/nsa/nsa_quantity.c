#include <string.h>
#include "nsa.h"

static int
gur_lookahead(struct nsa_parser *p)
{
  List_node *r = p->toks->rover;
  while (r && (((struct nsa_token*)r->data)->type == NSA_T_COUNT
	       || ((struct nsa_token*)r->data)->type == NSA_T_UNIT))
    {
      if (((struct nsa_token*)r->data)->type == NSA_T_UNIT
	  && (!strcmp(unit_name((struct nsa_token*)r->data),"barig")
	      || !strncmp(unit_name((struct nsa_token*)r->data),"ban",3)))
	return 1;
      r = r->next;
    }
  while (r && ((struct nsa_token*)r->data)->type == NSA_T_GRAPHEME)
    {
      if (!strcmp((const char *)nsa_grapheme_text(((struct nsa_token*)r->data)),"gur"))
	return 1;
      else
	r = r->next;
    }
  return 0;
}

static struct nsa_token *
igigal_trap(List *prevtoks, struct nsa_token *first, struct nsa_parser *p)
{
  if (first->type == NSA_T_GRAPHEME && !strcmp((const char *)nsa_grapheme_text(first),"igi"))
    {
      struct nsa_token *count = NULL, *gal2 = NULL;
      count = list_next(p->toks);
      if (count && count->type == NSA_T_COUNT)
	{
	  gal2 = list_next(p->toks);
	  if (gal2 
	      && gal2->type == NSA_T_GRAPHEME 
	      && hash_find(p->context->gal2_tokens,
			   (unsigned char *)nsa_trim_morph(p->context, (const char *)nsa_grapheme_text(gal2))))
	    {
	      struct nsa_token *newt = new_token(), *t, *newtu;
	      struct nsa_count *newc = new_count();

	      newt->type = NSA_T_COUNT;
	      newt->children = new_children(3);
	      newt->children[0] = first;
	      newt->children[1] = count;
	      newt->children[2] = gal2;
	      newt->d.c = newc;
	      count_num(newt) = 1;
	      count_den(newt) = (int)count_num(count);
	      t = list_next(p->toks);
	      if (t && t->type == NSA_T_UNIT)
		newtu = create_unit(p,unit_name(t),t); /*WATCHME: is this ever right 
							 with igigal?*/
	      else
		{
		  struct nsa_token *prev = NULL;
		  const char *igigal_u = NULL;
		  if (t)
		    (void)list_unnext(p->toks);
		  if ((prev = nsa_last_unit(prevtoks))
		      && (igigal_u = hash_find(p->context->igigal_keys,
					       (unsigned char *)unit_name(prev))))
		    {
		      newtu = create_unit(p,igigal_u,NULL);
		      unit_implicit(newtu) = 1;
		    }
		  else if ((prev = nsa_last_grapheme(prevtoks))
			   && (igigal_u = hash_find(p->context->igigal_keys,
						    nsa_grapheme_text(prev))))
		    {
		      newtu = create_unit(p,igigal_u,NULL);
		      unit_implicit(newtu) = 1;
		    }
		  else
		    {
		      struct nsa_token *next = nsa_next_grapheme(p->toks);
		      if (next && next->type == NSA_T_GRAPHEME 
			  && (igigal_u = hash_find(p->context->igigal_keys,
						   (unsigned char *)
						   nsa_trim_morph(p->context,
								  (const char *)
								  nsa_grapheme_text(next)))))
			{
			  newtu = create_unit(p,igigal_u,NULL);
			  unit_implicit(newtu) = 1;
			}
		      else
			{
			  newtu = NULL;
			}
		    }
		}
	      if (newtu)
		{
		  struct nsa_quantity *q = new_quantity();
		  struct nsa_token *newtq = new_token();
		  q->count = newt;
		  q->unit = newtu;
		  newtq->type = NSA_T_QUANTITY;
		  newtq->d.q = q;
		  return newtq;
		}
	      else
		{
		  
		  return newt;
		}
	    }
	  else
	    {
	      (void)list_unnext(p->toks);
	      if (gal2)
		(void)list_unnext(p->toks);
	      return first;
	    }
	}
      else
	{
	  if (count)
	    (void)list_unnext(p->toks);
	  return first;
	}
    }
  else
    return first;
}

void
nsa_quantity(struct nsa_parser *p)
{
  List *newtoks = list_create(LIST_DOUBLE);
  struct nsa_token *t;
  for (t = list_first(p->toks); t; t = list_next(p->toks))
    {
      if (t->type == NSA_T_COUNT && list_has_next(p->toks))
	{
	  struct nsa_token *t2 = list_next(p->toks);
	  if (t2->type == NSA_T_UNIT 
	      && (strcmp(unit_name(t2),"še") || count_axis(t) == nsa_sex_disz_axis))
	    {
	      struct nsa_token *newt = new_token();
	      struct nsa_quantity *q = new_quantity();
	      q->count = t;
	      q->unit = t2;
	      newt->type = NSA_T_QUANTITY;
	      newt->d.q = q;
	      list_add(newtoks,newt);
	    }
	  else
	    {
	      (void)list_unnext(p->toks);
	      if (count_axis(t) != nsa_sex_disz_axis && gur_lookahead(p))
		{
		  struct nsa_token *newt = new_token();
		  struct nsa_quantity *q = new_quantity();
		  count_axis(t) = nsa_sex_asz_axis;
		  q->count = t;
		  q->unit = create_unit(p,"*gur",NULL);
		  newt->type = NSA_T_QUANTITY;
		  newt->d.q = q;
		  list_add(newtoks,newt);
		}
	      else
		list_add(newtoks,t);
	    }
	}
      else if (t->type == NSA_T_GRAPHEME)
	list_add(newtoks,igigal_trap(newtoks,t,p));
      else
	list_add(newtoks,t);
    }
  list_free(p->toks,NULL);
  p->toks = newtoks;
}
