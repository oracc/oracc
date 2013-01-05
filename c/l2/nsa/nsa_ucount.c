#include <string.h>
#include "nsa.h"

static struct nsa_hash_data *
check_continuations(struct nsa_hash_data *d, struct nsa_parser *p, 
		    const char **n, List *ctoks)
{
  static char buf[1024];
  struct nsa_hash_data *last_terminal = NULL;
  int last_terminal_len = 0;
  *buf = '\0';

  while (1)
    {
      if (strlen(buf))
	strcat(buf," ");
      strcat(buf,*n);
      if (d->cands)
	{
	  last_terminal = d;
	  last_terminal_len = strlen(buf);
	}
      if (d->continuations)
	{
	  struct nsa_token *t = list_next(p->toks);
	  if (t && t->type == NSA_T_GRAPHEME && !grapheme_num(t))
	    {
	      const char *n2 = nsa_trim_morph(p->context, (const char *)nsa_grapheme_text(t));
	      struct nsa_hash_data *d2 = hash_find(d->continuations, (unsigned char*)n2);
	      if (d2)
		{
		  d = d2;
		  *n = n2;
		  list_add(ctoks,t);
		}
	      else
		{
		  (void)list_unnext(p->toks);
		  break;
		}
	    }
	  else if (t)
	    {
	      (void)list_unnext(p->toks);
	      break;
	    }
	  else
	    break;
	}
      else
	break;
    }
  if (!d->cands)
    {
      d = last_terminal;
      buf[last_terminal_len] = '\0';
    }
  *n = buf;
  return d;
}

struct nsa_token *
create_unit(struct nsa_parser *p,const char *s,struct nsa_token *t)
{
  const char *n = nsa_trim_morph(p->context, s);
  struct nsa_hash_data *d = hash_find(p->context->step_index, (unsigned char *)n);
  if (d)
    {
      struct nsa_token *tu = new_token();
      struct nsa_unit *u = new_unit();
      List *l = list_create(LIST_SINGLE);

      list_add(l,t);

      if (d->continuations)
	d = check_continuations(d,p,&n,l);

      u->name = (char *)npool_copy((unsigned char *)n,p->pool);
      u->cands = d->cands;
      tu->type = NSA_T_UNIT;
      if (t)
	{
	  struct nsa_token *lt;
	  int i;
	  tu->children = new_children(list_len(l));
	  for (i = 0, lt = list_first(l); lt; lt = list_next(l),++i)
	    tu->children[i] = lt;
	}
      tu->d.u = u;
      list_free(l,NULL);
      return tu;
    }
  else
    return t;
}

static struct nsa_token *
la2_trap(List *ntoks, struct nsa_parser *p)
{
  if (list_len(ntoks) 
      && ((struct nsa_token*)list_last(ntoks))->type == NSA_T_GRAPHEME
      && hash_find(p->context->la2_tokens,nsa_grapheme_text(list_last(ntoks))))
    return list_pop(ntoks);
  else
    return NULL;
}

void
nsa_ucount(struct nsa_parser *p)
{
  List *newtoks = list_create(LIST_DOUBLE);
  struct nsa_token *t;
  for (t = list_first(p->toks); t; t = list_next(p->toks))
    {
      if (t->type == NSA_T_GRAPHEME)
	{
	  if (grapheme_num(t))
	    {
	      struct nsa_token *c = new_token();
	      struct nsa_token *la2 = la2_trap(newtoks,p);
	      int nkids = 1;
	      if (la2)
		++nkids;
	      c->d.c = new_count();
	      c->type = NSA_T_COUNT;
	      c->children = new_children(nkids);
	      if (la2)
		{
		  c->children[0] = la2;
		  c->children[1] = t;
		}
	      else
		c->children[0] = t;
	      
	      *c->d.c = *nsa_parse_count(grapheme_num(t),la2 ? -1 : 1);
	      list_add(newtoks,c);
	      if (grapheme_unit(t))
		{
		  struct nsa_token *tu = create_unit(p, grapheme_unit(t), NULL);
		  if (tu)
		    list_add(newtoks,tu);
		  else
		    fprintf(stderr,"unknown unit in count-unit grapheme `%s'\n",
			    nsa_grapheme_text(t));
		}
	    }
	  else if (!newtoks->last
		   || ((struct nsa_token*)list_last(newtoks))->type != NSA_T_UNIT)
	    list_add(newtoks,create_unit(p, (const char *)nsa_grapheme_text(t), t));
	  else
	    list_add(newtoks,t);
	}
      else
	list_add(newtoks,t);
    }
  list_free(p->toks,NULL);
  p->toks = newtoks;
}
