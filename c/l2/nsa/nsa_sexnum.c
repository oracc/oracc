#include <string.h>
#include "nsa.h"

const char const *sex_axis_str[] = { "diš", "aš", NULL };
const char const *sex_form_str[] = { "curviform", "cuneiform", NULL };

static enum nsa_sex_axis
sex_axis(const char *n)
{
  if (strstr(n,"di"))
    return nsa_sex_disz_axis;
  else
    return nsa_sex_asz_axis;
}

static enum nsa_sex_form
sex_form(const char *n)
{
  if (strstr(n,"@c"))
    return nsa_sex_curviform;
  else
    return nsa_sex_cuneiform;
}

static struct nsa_token *
sexfrac_trap(struct nsa_parser *p)
{
  struct nsa_token *t = list_next(p->toks);
  if (t && t->d.g)
    {
      if (hash_find(p->context->sexfracs, nsa_grapheme_text(t)))
	return t;
      else
	(void)list_unnext(p->toks);
    }
  return NULL;
}

static struct nsa_token *
collect_sextoks(struct nsa_sex_tab *s, struct nsa_token *t, struct nsa_parser *p)
{
  List *sextoks = list_create(LIST_SINGLE);
  int last_value = 0, sex_sign = 1;
  enum nsa_sex_axis axis = nsa_sex_no_axis;
  enum nsa_sex_form form = nsa_sex_no_form;
  struct nsa_count *accumulator = new_count();
  struct nsa_token *nc;
  int i;

  if (s)
    {
      last_value = s->value;
      form = sex_form(s->name);
      if (s->value == 1 && axis == nsa_sex_no_axis)
	axis = sex_axis(s->name);
      *accumulator = *(nsa_parse_count(grapheme_num(t),1));
      accumulator->num *= s->value;
    }
  else
    {
      sex_sign = -1;
      last_value = 1000000;
    }

  list_add(sextoks,t);

  for (t = list_next(p->toks); t; t = list_next(p->toks))
    {
      struct nsa_sex_tab *s = NULL;
      if (t->type == NSA_T_GRAPHEME && grapheme_num(t) && grapheme_unit(t) 
	  && (s = sexnum(grapheme_unit(t),strlen(grapheme_unit(t)))) 
	  && (s->value < last_value || (s->value == last_value 
					&& strchr(grapheme_num(t),'/')))
	  && (form == nsa_sex_no_form || form == sex_form(s->name)))
	{
	  struct nsa_count *c2;
	  
	  list_add(sextoks,t);
	  if (s->value == 1 && axis == nsa_sex_no_axis)
	    axis = sex_axis(s->name);
	  c2 = nsa_parse_count(grapheme_num(t),sex_sign);
	  c2->num *= s->value;
	  if (accumulator->den)
	    nsa_add_frac(accumulator,c2);
	  else
	    *accumulator = *c2;
	  if (sex_sign == -1)
	    sex_sign = 1;
	}
      else if (list_len(sextoks)
	       && t->type == NSA_T_GRAPHEME
	       && hash_find(p->context->la2_tokens,nsa_grapheme_text(t)))
	{
	  /*la2_tok = t;*/
	  list_add(sextoks,t);
	  sex_sign = -1;
	}
      else
	{
	  (void)list_unnext(p->toks);
	  break;
	}
    }

  nc = new_token();
  nc->type = NSA_T_COUNT;
  nc->children = new_children(list_len(sextoks));
  for (i = 0, t = list_first(sextoks); t; t = list_next(sextoks), ++i)
    nc->children[i] = t;
  list_free(sextoks,NULL);
  accumulator->base = 60;
  accumulator->axis = axis;
  accumulator->form = form;
  nc->d.c = accumulator;
  return nc;
}

void
nsa_sexnum(struct nsa_parser *p)
{
  List *newtoks = list_create(LIST_DOUBLE);
  struct nsa_token *t;
  for (t = list_first(p->toks); t; t = list_next(p->toks))
    {
      struct nsa_sex_tab *s = NULL;
      struct nsa_token *sexfrac = NULL;
      
      if (t->type == NSA_T_GRAPHEME && grapheme_num(t) && grapheme_unit(t) 
	  && (s = sexnum(grapheme_unit(t),strlen(grapheme_unit(t)))))
	{
	  struct nsa_token *sexcount = collect_sextoks(s,t,p);
	  if (list_len(newtoks) 
	      && ((struct nsa_token*)list_last(newtoks))->type == NSA_T_COUNT
	      && (sexfrac = sexfrac_trap(p)))
	    {
	      struct nsa_token *prevcount = list_last(newtoks);
	      nsa_append_child(sexcount,sexfrac);
	      nsa_append_child(prevcount,sexcount);
	      count_den(sexcount) = (strchr((const char *)nsa_grapheme_text(sexfrac),'i')
				     ? 60 : 10800);
	      nsa_add_frac(prevcount->d.c,sexcount->d.c);
	    }
	  else
	    list_add(newtoks, sexcount);
	}
      else
	list_add(newtoks,t);
    }
  list_free(p->toks, NULL);
  p->toks = newtoks;
}
