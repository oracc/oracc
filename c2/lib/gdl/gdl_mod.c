#include <stdlib.h>
#include <ctype.h>
#include <mesg.h>
#include <list.h>
#include <tree.h>
#include "gdl.h"

static List *modq;

/* Mod nodes attached to the current grapheme ynp */
Node *
gdl_mod(Tree *ytp, const char *data)
{
  Node *np = NULL;
  const char *n = NULL;
  int mpushed = 0;

  /* if we have )@c we need to add to the extant parent and not push;
     this may also need to test for g:q nodes but it may be that all
     g:q nodes that exhibit this behaviour are actually g:n nodes and
     get converted to g:n before gdl_mod is called in this context */
  if (strcmp(ytp->curr->last->name, "g:gp")
      && strcmp(ytp->curr->name, "g:n")
      && 'M' != ytp->curr->last->name[2]
      && 'A' != ytp->curr->last->name[2])
    mpushed = 1;

  switch (*data++)
    {
    case '@':
      n = mpushed ? "g:m" : "g:M";
      break;
    case '~':
      n = mpushed ? "g:a" : "g:A";
      break;
    case '\\':
      n = mpushed ? "g:f" : "g:F";
      break;
    default:
      fprintf(stderr, "gdl_mod: internal error: unknown mod %s\n", data-1);
      break;
    }

  if (gdltrace)    
    fprintf(stderr, "gt: MOD: %s\n", data);

  if (mpushed)
    (void)tree_push(ytp);
    
  if (!ytp->curr->kids)
    {
      np = tree_add(ytp, NS_GDL, ytp->curr->name, ytp->curr->depth+1, NULL);
      np->text = (ccp)pool_copy((uccp)ytp->curr->text,gdlpool);
      np->mloc = np->rent->mloc;
      /*np->name = "g:b";*/
      /* This is done in a Bison rule which this node-copy won't be
	 processed by so we have to unlegacy here */
      if (gdl_legacy)
	gdl_unlegacy(np);
    }
  np = tree_add(ytp, NS_GDL, n, ytp->curr->depth+1, NULL);
  np->text = (ccp)pool_copy((uccp)data,gdlpool);
  np->mloc = np->rent->mloc;

  if (mpushed)
    tree_pop(ytp);

  /* return the node that wraps the mod */
  return np;
}

/* A sign with mods has a copy of the sign in a g:b(ase) node and the
   mods as kids.  Process the child version of the sign, build the
   full form and make that the current node's text, then process the
   top-level sign+mods combo.

   To preserve original input in the case of GANA₂@t, which is c10ed
   to GAN₂@t, we rebuild both the original and c10e forms and reset
   top-level gp->orig to the built orig.

   For |(LAK079&LAK079)@g| the mod has been added to the group not
   LAK079. In this case ynp->kids is NULL and we return without
   further processing.
*/
void
gdl_mod_wrap(Node *ynp, int sub_simplexg)
{
  List *op = NULL, *cp = NULL;
  Node *np = NULL;
  unsigned char *o = NULL, *c = NULL;
  gvl_g*gp = NULL;

  if (NULL == ynp->kids)
    return;

  op = list_create(LIST_SINGLE);
  cp = list_create(LIST_SINGLE);

  gvl_simplexg(ynp->kids);
  
  for (np = ynp->kids; np; np = np->next)
    {
      if (np->user && ((gvl_g*)np->user)->orig)
	{
	  np->name = "g:b";
	  list_add(op, (void*)((gvl_g*)np->user)->orig);
	  if (!gdl_orig_mode && ((gvl_g*)np->user)->c10e)
	    list_add(cp, (void*)((gvl_g*)np->user)->c10e);
	  else
	    list_add(cp, (void*)((gvl_g*)np->user)->orig);
	}
      else
	{
	  if (np->name[2] == 'm' || np->name[2] == 'M')
	    {
	      list_add(op, "@");
	      list_add(cp, "@");
	    }
	  else if (np->name[2] == 'a' || np->name[2] == 'A')
	    {
	      list_add(op, "~");
	      list_add(cp, "~");
	    }
	  if (np->name[2] != 'f' || np->name[2] == 'F')
	    {
	      list_add(op, (void*)np->text);
	      list_add(cp, (void*)np->text);
	    }
	}
    }
  c = list_concat(cp);
  o = list_concat(op);
  if (!(gp = hash_find(curr_sl->h,(uccp)c))
      && !(gp = hash_find(curr_sl->h,(uccp)o)))
    {
      ynp->user = gp = memo_new(curr_sl->m);
      gp->type = ynp->name + 2;
      ((gvl_g*)ynp->user)->c10e = pool_copy(c,curr_sl->p);
      if (strcmp((ccp)c, (ccp)o))
	((gvl_g*)ynp->user)->orig = pool_copy(o,curr_sl->p);
      else
	((gvl_g*)ynp->user)->orig = ((gvl_g*)ynp->user)->c10e;
      if (gvl_lookup(c) && !gdl_orig_mode)
	ynp->text = (ccp)((gvl_g*)ynp->user)->c10e;
      else
	ynp->text = (ccp)((gvl_g*)ynp->user)->orig;
      /* need to review hash_add throughout */
    }
  list_free(cp, NULL);
  list_free(op, NULL);

  /* For valuqal type q gdl_mod_wrap_q attaches the mods to the top
     level; if gdl_mod_wrap is being called on a sign that is a child
     of g:n we need to update the top-level here */
  if (ynp->rent->name[2] == 'n')
    {
      gvl_g *ng = memo_new(curr_sl->m); /* update in a new gvl_g */
      /*gvl_g *rg = ynp->rent->user;*/
      gvl_g *gg = ynp->user;

      const char *r = ynp->rent->kids->text;
      o = pool_alloc(strlen(r) + strlen((ccp)gg->orig) + 3, curr_sl->p);
      sprintf((char*)o, "%s(%s)", r, gg->orig);
      c = pool_alloc(strlen(r) + strlen((ccp)gg->c10e) + 3, curr_sl->p);
      sprintf((char*)c, "%s(%s)", r, gg->orig);

      ng->orig = o;
      if (gvl_lookup(c))
	{
	  if (gdl_orig_mode)
	    ynp->rent->text = (ccp)o;
	  else	    
	    ynp->rent->text = (ccp)c;
	  ng->c10e = c;
	}
      else
	{
	  ng->mess = gvl_vmess("unknown sign name: %s", ng->orig);
	  ynp->rent->text = (ccp)o;
	  ng->c10e = o;
	}
      ynp->rent->user = ng;
    }
}

void
gdl_modq_init(void)
{
  modq = list_create(LIST_SINGLE);
}

void
gdl_modq_flush(void)
{
  Node *last = NULL;
  Node *curr = NULL;
  for (curr = list_first(modq); curr; curr = list_next(modq))
    {
      /* don't process if the parent of mods is a g:c because the mods
	 are already in the right place */
      if ('c' != curr->name[2])
	{
	  if (curr != last)
	    {
	      last = curr;
	      if ('n' == curr->rent->name[2])
		gdl_mod_wrap_q(curr->rent);
	      else if ('c' != curr->rent->name[2] && strcmp(curr->rent->name, "g:gp"))
		(void)gdl_mod_wrap(curr->rent, 0);
	    }
	}
    }
  list_reset(modq);
}

void
gdl_modq_term(void)
{
  list_free(modq, NULL);
}

void
gdl_modq_add(Node *np)
{
  list_add(modq, np);
}

void
gdl_mod_wrap_q(Node *np)
{
  List *mp = list_create(LIST_SINGLE);
  const char *res = "";
  Node *tmp;
  
  for (tmp = np->kids; tmp; tmp = tmp->next)
    {
      if (tmp->name[2] == 'm' || tmp->name[2] == 'M')
	{
	  tmp->name = "g:m";
	  list_add(mp, "@");
	  list_add(mp, (void*)tmp->text);
	}
      else if (tmp->name[2] == 'a' || tmp->name[2] == 'A')
	{
	  tmp->name = "g:a";
	  list_add(mp, "~");
	  list_add(mp, (void*)tmp->text);
	}
      else if (tmp->name[2] == 'f' || tmp->name[2] == 'F')
	{
	  tmp->name = "g:f";
	  /* ignore graphetics */
	}
      else
	{
	  /* only cat mod nodes */
	}
    }  
  if (list_len(mp))
    res = (ccp)list_concat(mp);

  list_free(mp, NULL);
  if (res && *res)
    {
      unsigned char *o = NULL, *c = NULL;
      if (np->user)
	{
	  gvl_g *ng = memo_new(curr_sl->m); /* update in a new gvl_g */
	  o = pool_alloc(strlen((ccp)((gvl_g*)np->user)->orig) + strlen(res) + 1, gdlpool);
	  sprintf((char*)o, "%s%s", (ccp)((gvl_g*)np->user)->orig, (ccp)res);

	  if (((gvl_g*)np->user)->c10e)
	    {
	      c = pool_alloc(strlen((ccp)((gvl_g*)np->user)->c10e) + strlen(res) + 1, gdlpool);
	      sprintf((char*)c, "%s%s", ((gvl_g*)np->user)->orig, res);
	    }

	  if (c && gvl_lookup(c))
	    {
	      if (gdl_orig_mode)
		np->text = (ccp)o;
	      else
		np->text = (ccp)c;
	      ng->c10e = c;
	      ng->orig = o;
	      np->user = ng;
#if 0	      
	      ((gvl_g*)np->user)->c10e = c;
	      ((gvl_g*)np->user)->orig = o;
#endif
	    }
	  else
	    {
	      np->text = (ccp)o;
	      ng->orig = ng->c10e = o;
	      np->user = ng;
	      /*((gvl_g*)np->user)->orig = ((gvl_g*)np->user)->c10e = o; */
	    }
	}  
    }
}
