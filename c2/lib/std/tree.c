#include <stdlib.h>
#include <stdio.h>
#include "pool.h"
#include "memo.h"
#include "tree.h"

Tree *
tree_init(void)
{
  Tree *tp = calloc(1, sizeof(Tree));
  tp->nodemem = memo_init(sizeof(Node), 1024);
  tp->pool = pool_init();
  return tp;
}

void
tree_term(Tree *tp)
{
  memo_term(tp->nodemem);
  pool_term(tp->pool);
  free(tp);
}

Node *
tree_node(Tree *tp, const char *name, int depth, Mloc *loc)
{
  Node *np = memo_new(tp->nodemem);
  np->name = name;
  np->depth = depth;
  np->mloc = loc;
  return np;
}

Node *
tree_root(Tree *tp, const char *name, int depth, Mloc *loc)
{
  Node *np = tree_node(tp, name, depth, loc);
  tp->root = tp->curr = np;
  return np;
}

Node *
tree_add(Tree *tp, const char *name, int depth, Mloc *loc)
{
  Node *np = NULL;

  if (tp)
    {
      if (tp->curr && tp->curr->kids)
	{
	  np = NULL;
	  np = tree_node(tp, name, depth, loc);
	  np->rent = tp->curr->last->rent;
	  tp->curr->last->next = np;
	  tp->curr->last = np;
	}
      else
	{
	  np = tree_node(tp, name, depth, loc);
	  if (!tp->curr)
	    {
	      tp->curr = tp->root = np;
	      /* leave tp->rent and tp->kids as NULL */
	    }
	  else
	    {
	      np->rent = tp->curr;
	      tp->curr->last = tp->curr->kids = np;
	    }
	}
    }
  return np;
}

Node *
tree_pop(Tree *tp)
{
  if (tp->curr->rent)
    return tp->curr = tp->curr->rent;
  else
    return NULL;
}

void
tree_push(Tree *tp)
{
  if (tp->curr->last)
    tp->curr = tp->curr->last;
}

static void
_do_node(Node *np, void *user, void (*nodefnc)(Node *np, void *user), void (*postfnc)(Node *np, void *user))
{
  while (np)
    {
      nodefnc(np, user);
      for (np = np->kids; np; np = np->next)
	_do_node(np, user, nodefnc, postfnc);
      postfnc(np, user);
    }     
}

void
tree_iterator(Tree *tp, void *user, void (*nodefnc)(Node *np, void *user), void (*postfnc)(Node *np, void *user))
{
  if (tp && tp->root)
    _do_node(tp->root, user, nodefnc, postfnc);
}
