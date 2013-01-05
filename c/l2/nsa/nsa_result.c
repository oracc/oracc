#include "nsa.h"

struct xl_info
{
  struct nsa_text_ref *text;
  const char *role;
};

int xlink_results = 0;

#define t_is_quant(x) \
  ((x)->d.a->measure->children[0]->type==NSA_T_QUANTITY)

#define first_lemm_ptr(x) \
  ((x)->d.a->measure->children[0]->d.q->count->children[0]->d.g->text->t.lemmptr)

#define first_lemm_ptr_c(x) \
  ((x)->d.a->measure->children[0]->children[0]->d.g->text->t.lemmptr)

#define sysname(x) \
  (x)->d.a->measure->d.m->system->n

static const char *
countbase_str(struct nsa_token *m, struct nsa_parser *p)
{
  if (m->type == NSA_T_COUNT)
    {
      char buf[128];
      sprintf(buf,"%d",count_base(m));
      return (const char *)npool_copy((unsigned char *)buf,p->pool);
    }
  return "";
}

struct nsa_result *
nsa_create_result(struct nsa_parser *p)
{
  struct nsa_token *t;
  struct nsa_result *r = new_result();
  struct nsa_result_chunk *tmp;
  List *rchunks = list_create(LIST_SINGLE);
  int i;
  
  r->success = 1;
  r->ambigs = 0;
  r->parser = p;
  for (t = list_first(p->toks); t; t = list_next(p->toks))
    {
      struct nsa_result_chunk *rc = new_result_chunk();
      List *g;
      struct xl_info *xip;
      int i;
      if (t->type != NSA_T_AMOUNT)
	{
	  /* decompose any partially analyzed elements back into graphemes */
	  g = get_text_refs(t,p->toks,NSA_T_AMOUNT);
	  rc->type = NSA_R_UNPARSED;
	  rc->amount = NULL;
	}
      else
	{
	  g = get_text_refs(t,NULL,NSA_T_NONE);
	  rc->type = NSA_R_PARSED;
	  rc->amount = t->d.a;
	  if (xlink_results)
	    {
	      struct linkset *lsp;
	      const char *override = nsa_system_override_get();
	      if (override)
		{
		  if (amount_measure(t)->type == NSA_T_MEASURE)
		    {
		      t->d.a->measure->d.m->system = new_system();
		      t->d.a->measure->d.m->system->n = override;
		      if (t->d.a->measure->d.m->cands)
			list_free(t->d.a->measure->d.m->cands, NULL);
		    }
		}
	      if (amount_measure(t)->type == NSA_T_MEASURE && t->d.a->measure->d.m->cands)
		{
		  nsa_xcl_warning(p->context->user,
				  t_is_quant(t) ? first_lemm_ptr(t) : first_lemm_ptr_c(t),
				  "ambiguous system: %s", 
				  show_sys_list_str(t->d.a->measure->d.m->cands));
		}
	      else if (amount_measure(t)->type == NSA_T_MEASURE)
		{
		  int ok = 0;
		  if (measure_system(amount_measure(t)) && amount_commodity(t))
		    {
		      List *l;
		      if ((l = hash_find(p->context->comheads,
					 nsa_grapheme_text(t->d.a->commodity->children[0]))))
			{
			  const char *s;
			  ok = -1;
			  for (s = list_first(l); s; s = list_next(l))
			    if (!strcmp(s, sysname(t)))
			      {
				ok = 1;
				break;
			      }
			  if (ok == -1)
			    {
			      if (t->d.a->commodity)
				nsa_xcl_warning(p->context->user,
						t_is_quant(t) ? first_lemm_ptr(t) : first_lemm_ptr_c(t),
						"commodity head `%s' not known for system `%s'",
						nsa_grapheme_text(t->d.a->commodity->children[0]),
						sysname(t));
			    }
			}
		    }
		  if (ok == 0)
		    {
		      const char *pregraph;
		      struct nsa_token **kids;
		      int i;
		      if (amount_measure(t)->type == NSA_T_MEASURE)
			kids = quantity_count(amount_measure(t)->children[0])->children;
		      else
			kids = amount_measure(t)->children;
		      for (i = 0; kids[i]; ++i)
			;
		      pregraph = (const char *)nsa_grapheme_text(kids[i-1]);
		      if (t->d.a->commodity)
			nsa_xcl_warning(p->context->user,
					t_is_quant(t) ? first_lemm_ptr(t) : first_lemm_ptr_c(t),
					"commodity head `%s' unknown; say `%s , %s' if correct",
					nsa_grapheme_text(t->d.a->commodity->children[0]),
					pregraph, nsa_grapheme_text(t->d.a->commodity->children[0])
					);
		    }
		}

	      /* FIXME: we assume for now that measured commodities
		 are not countable; we need to maintain a positive
		 list of countables */
	      else if (amount_measure(t)->type == NSA_T_COUNT 
		       && amount_commodity(t)
		       && commodity_cands(amount_commodity(t)))
		{
		  List *l = hash_find(p->context->comheads,
				      nsa_grapheme_text(t->d.a->commodity->children[0]));
		  int ok = 0;
		  if (l)
		    {
		      const char *s;
		      for (s = list_first(l); s; s = list_next(l))
			if (!strcmp(s, "sexnum"))
			  {
			    ok = 1;
			    break;
			  }
		    }
		  if (!ok)
		    {
		      nsa_xcl_warning(p->context->user,
				      amount_commodity(t)->children[0]->d.g->text->t.lemmptr,
				      "commodity `%s' should be measured not counted",
				      nsa_grapheme_text(t->d.a->commodity->children[0]));
		      commodity_cands(amount_commodity(t)) = NULL;
		    }
		  else if (count_axis(amount_measure(t)) == nsa_sex_asz_axis)
		    nsa_xcl_warning(p->context->user,
				    amount_commodity(t)->children[0]->d.g->text->t.lemmptr,
				    "strange: commodity `%s' is counted in ASZ",
				    nsa_grapheme_text(t->d.a->commodity->children[0]));
		}

	      /* If an NSA_T_AMOUNT without a commodity is followed immediately by another
		 NSA_T_AMOUNT this is an error; the field separator ',' can be used to 
		 suppress this warning */	
	      if (p->toks->rover->next 
		  && ((struct nsa_token *)p->toks->rover->next->data)->type == NSA_T_AMOUNT
		  && !amount_commodity(t))
		{
		  const char *pregraph, *postgraph;
		  struct nsa_token **kids, *ntok = (struct nsa_token *)p->toks->rover->next->data;
		  int i;
		  if (amount_measure(t)->type == NSA_T_MEASURE)
		    {
		      int j;
		      for (j = 0; amount_measure(t)->children[j]; ++j)
			;
		      kids = quantity_count(amount_measure(t)->children[j-1])->children;
		    }
		  else
		    kids = amount_measure(t)->children;
		  for (i = 0; kids[i]; ++i)
		    ;
		  pregraph = (const char *)nsa_grapheme_text(kids[i-1]);
		  if (amount_measure(ntok)->type == NSA_T_MEASURE)
		    kids = quantity_count(amount_measure(ntok)->children[0])->children;
		  else
		    kids = amount_measure(ntok)->children;
		  postgraph = (const char*)nsa_grapheme_text(kids[0]);
		  nsa_xcl_warning(p->context->user,
				  kids[0]->d.g->text->t.lemmptr,
				  "suspicious sequence `%s %s'; lemmatize `n; %% n' if correct",
				  pregraph,postgraph,pregraph,postgraph);
		}
	      if (amount_measure(t)->type == NSA_T_MEASURE)
		{
		  lsp = new_linkset(((struct xcl_context*)p->context->user)->linkbase,
				    "nsa", 
				    (measure_system(amount_measure(t)))
				    ? sysname(t)
				    : ((measure_cands(amount_measure(t)))
					? show_sys_list_str(measure_cands(amount_measure(t)))
				       : "#unknown system#"));
		  lsp->user = nsa_xcl_info(t->d.a);
		  lsp->user_dump_function = (user_dump_func*)nsa_xcl_dump;
		}
	      else
		{
		  lsp = new_linkset(((struct xcl_context*)p->context->user)->linkbase,
				    "nsa", countbase_str(amount_measure(t),p));
		  if (t->type == NSA_T_AMOUNT)
		    {
		      lsp->user = nsa_xcl_info(t->d.a);
		      lsp->user_dump_function = (user_dump_func*)nsa_xcl_dump;
		    }
		  else
		    {
		      lsp->user = NULL;
		    }
		}
	      preallocate_links(lsp,list_len(g));
	      for (i = 0, xip = list_first(g); xip; xip = list_next(g), ++i)
		{
		  struct nsa_text_ref *pg = xip->text;
		  struct link *l = &lsp->links[i];
		  l->lref = pg->t.lemmptr->xml_id;
		  l->lp = pg->t.lemmptr;
		  l->role = xip->role;
		  l->title = (char*)pg->t.lemmptr->f->f2.form;
		  ++lsp->used;
		}

	      /* FUTURE: we can emit commodity links as well with l->role = "c" */

	      /* and we should really emit sysdet links, too */
	    }
	  else if (amount_measure(t)->type == NSA_T_MEASURE && t->d.a->measure->d.m->cands)
	    ++r->ambigs;
	}
      rc->refs = malloc((list_len(g)+1) 
			* sizeof(struct nsa_text_ref*));
      if (!rc->refs)
	nsa_mem_die();
      for (i = 0, xip = list_first(g); xip; xip = list_next(g), ++i)
	rc->refs[i] = xip->text;
      rc->refs[i] = NULL;
      list_free(g,NULL);
      list_add(rchunks,rc);
    }
  r->results = malloc((list_len(rchunks)+1)*sizeof(struct nsa_result_chunk*));
  if (!r->results)
    nsa_mem_die();
  for (i = 0, tmp = list_first(rchunks); tmp; tmp = list_next(rchunks), ++i)
    r->results[i] = tmp;
  r->results[i] = NULL;
  list_free(rchunks,NULL);
  nsa_system_override_set(NULL);
  return r;
}

List *
get_text_refs(struct nsa_token *first_t, List *toks, enum nsa_ttypes stop_type)
{
  List *g = list_create(LIST_SINGLE);
  struct nsa_token *t = first_t;
  while (1)
    {
      if (!t)
	break;
      else if (t->type == stop_type)
	{
	  if (toks)
	    toks->rover = toks->rover->prev;
	  break;
	}
      else
	{
	  find_refs(t,g,"num");
	  if (toks)
	    t = list_next(toks);
	  else
	    break;
	}
    }
  return g;
}

void
find_refs(struct nsa_token *t, List *g, const char *role)
{
  int i;
  switch (t->type)
    {
    case NSA_T_GRAPHEME:
      if (t->d.g->text)
	{
	  struct xl_info *xip = malloc(sizeof(struct xl_info));
	  xip->text = t->d.g->text;
	  xip->role = role;
	  list_add(g,xip);
	}
      break;
    case NSA_T_COUNT:
    case NSA_T_UNIT:
      if (t->children)
	for (i = 0; t->children[i]; ++i)
	  find_refs(t->children[i],g,"num");
      break;
    case NSA_T_QUANTITY:
      find_refs(t->d.q->count,g,"num");
      if (t->d.q->unit)
	find_refs(t->d.q->unit,g,"num");
      break;
    case NSA_T_MEASURE:
      if (t->children)
	for (i = 0; t->children[i]; ++i)
	  find_refs(t->children[i],g,"num");
      /* don't process sysdet here; must do it after commodity in amount */
      break;
    case NSA_T_COMMODITY:
#if 1
      if (t->children)
	for (i = 0; t->children[i]; ++i)
	  find_refs(t->children[i],g, "cty");
#else
      /* no processing necessary */
#endif
      break;
    case NSA_T_AMOUNT:
      find_refs(t->d.a->measure, g, "num");
      if (t->d.a->commodity)
	/* this needs to process the PSU commodity */
	find_refs(t->d.a->commodity,g, "cty");
      if (t->d.a->measure->type == NSA_T_MEASURE && t->d.a->measure->d.m->sysdet)
	find_refs(t->d.a->measure->d.m->sysdet,g, "sys");
      break;
    case NSA_T_STOP:
    case NSA_T_UNKNOWN:
    case NSA_T_NONE:
      break;
    }
}
