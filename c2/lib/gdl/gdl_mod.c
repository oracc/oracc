#include <stdlib.h>
#include <mesg.h>
#include <list.h>
#include <tree.h>
#include "gdl.h"

static List *modq;

/* Mod nodes attached to the current grapheme ynp */
void
gdl_mod(Tree *ytp, const char *data)
{
  Node *np = NULL;
  const char *n = NULL;
  switch (*data++)
    {
    case '@':
      n = "g:m";
      break;
    case '~':
      n = "g:a";
      break;
    case '\\':
      n = "g:f";
      break;
    default:
      fprintf(stderr, "gdl_mod: internal error: unknown mod %s\n", data-1);
      break;
    }
  if (gdltrace)    
    fprintf(stderr, "gt: MOD: %s\n", data);
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
  tree_pop(ytp);
}

/* A sign with mods has a copy of the sign in a g:b(ase) node and the
   mods as kids.  Process the child version of the sign, build the
   full form and make that the current node's text, then process the
   top-level sign+mods combo.

   To preserve original input in the case of GANA₂@t, which is c10ed
   to GAN₂@t, we rebuild both the original and c10e forms and reset
   top-level gp->orig to the built orig
*/
void
gdl_mod_wrap(Node *ynp, int sub_simplexg)
{
  List *op = list_create(LIST_SINGLE), *cp = list_create(LIST_SINGLE);
  Node *np = NULL;
  unsigned char *s = NULL;
  gvl_g*gp = NULL;

  gvl_simplexg(ynp->kids);
  
  for (np = ynp->kids; np; np = np->next)
    {
      if (np->user && ((gvl_g*)np->user)->c10e)
	{
	  np->name = "g:b";
	  list_add(op, (void*)((gvl_g*)np->user)->orig);
	  list_add(cp, (void*)((gvl_g*)np->user)->c10e);
	}
      else
	{
	  if (np->name[2] == 'm')
	    {
	      list_add(op, "@");
	      list_add(cp, "@");
	    }
	  else if (np->name[2] == 'a')
	    {
	      list_add(op, "~");
	      list_add(cp, "~");
	    }
	  if (np->name[2] != 'f')
	    {
	      list_add(op, (void*)np->text);
	      list_add(cp, (void*)np->text);
	    }
	}
    }
  s = list_concat(cp);
  ynp->text = (ccp)pool_copy(s,curr_sl->p);
  if (!(gp = hash_find(curr_sl->h,(uccp)ynp->text)))
    {
      ynp->user = gp = memo_new(curr_sl->m);
      gp->c10e = (uccp)ynp->text;
      gp->type = ynp->name + 2;
      s = list_concat(op);
      if (strcmp((ccp)s, ynp->text))
	((gvl_g*)ynp->user)->orig = pool_copy(s,curr_sl->p);
      else
	((gvl_g*)ynp->user)->orig = (uccp)ynp->text;
      /* need to review hash_add throughout */
    }
  list_free(cp, NULL);
  list_free(op, NULL);
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
      if (curr != last)
	{
	  /*unsigned const char *m_orig = NULL;*/
	  last = curr;
	  /*m_orig = */ (void)gdl_mod_wrap(curr, 0);
	  /* depending on result of code review possibly reset curr->orig <--m_orig here */
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
 
