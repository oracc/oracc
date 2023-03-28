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
  tp->propmem = memo_init(sizeof(Prop), 1024);
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
  np->tree = tp;
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
  if (np)
    {
      Node *entry_np = np;
      if (nodefnc)
	nodefnc(np, user);
      for (np = np->kids; np; np = np->next)
	_do_node(np, user, nodefnc, postfnc);
      if (postfnc)
	postfnc(entry_np, user);
    }
}

void
tree_iterator(Tree *tp, void *user, void (*nodefnc)(Node *np, void *user), void (*postfnc)(Node *np, void *user))
{
  if (tp && tp->root)
    _do_node(tp->root, user, nodefnc, postfnc);
}

void
node_iterator(Node *np, void *user, void (*nodefnc)(Node *np, void *user), void (*postfnc)(Node *np, void *user))
{
  _do_node(np, user, nodefnc, postfnc);
}

void
kids_add_node(Tree *tp, Node *np)
{
  if (tp && tp->curr)
    {
      if (tp->curr->kids)
	{
	  np->rent = tp->curr->last->rent;
	  tp->curr->last->next = np;
	  tp->curr->last = np;
	}
      else
	{
	  np->rent = tp->curr;
	  tp->curr->last = tp->curr->kids = np;
	}
    }
}

Node *
kids_rem_last(Tree *tp)
{
  Node *np = NULL;
  if (tp && tp->curr)
    {
      if (tp->curr->kids)
	{
	  Node *kp = tp->curr->kids;
	  if (kp->next)
	    {
	      while (kp->next != tp->curr->last)
		kp = kp->next;
	      tp->curr->last = kp;
	      np = kp->next;
	      kp->next = NULL;
	    }
	  else
	    {
	      np = kp;
	      kp->rent->kids = NULL;
	    }
	  np->rent = NULL;
	}
    }
  return np;
}

void
nodehandler_register(nodehandlerset nh, nodetype nt, nodehandler fnc)
{
  nh[nt] = fnc;
}
