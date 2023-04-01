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
tree_node(Tree *tp, nscode ns, const char *name, int depth, Mloc *loc)
{
  Node *np = memo_new(tp->nodemem);
  np->tree = tp;
  np->name = name;
  np->depth = depth;
  np->mloc = loc;
  np->ns = ns;
  if (!tp->ns_used[ns])
    tp->ns_used[ns] = ns;
  return np;
}

void
tree_ns_default(Tree *tp, nscode ns)
{
  tp->ns_used[0] = ns;
}

void
tree_ns_merge(Tree *tp, nsrefs used)
{
  enum nscode ns;
  for (ns = NS_NONE; ns < NS_LAST; ++ns)
    if (used[ns] && !tp->ns_used[ns])
      tp->ns_used[ns] = ns;
}

Node *
tree_root(Tree *tp, nscode ns, const char *name, int depth, Mloc *loc)
{
  Node *np = tree_node(tp, ns, name, depth, loc);
  tp->root = tp->curr = np;
  return np;
}

Node *
tree_add(Tree *tp, nscode ns, const char *name, int depth, Mloc *loc)
{
  Node *np = NULL;

  if (tp)
    {
      /* the case tp->curr->last == NULL indicates that tp->curr->kids
	 is a graft from another tree */
      if (tp->curr && tp->curr->kids && tp->curr->last)
	{
	  np = tree_node(tp, ns, name, depth, loc);
	  np->rent = tp->curr->last->rent;
	  tp->curr->last->next = np;
	  tp->curr->last = np;
	}
      else
	{
	  np = tree_node(tp, ns, name, depth, loc);
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

Node *
tree_push(Tree *tp)
{
  if (tp->curr->last)
    return tp->curr = tp->curr->last;
  else
    return NULL;
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

/* Graft the kids of tree tp onto node np */
void
tree_graft(Node *np, Tree *tp)
{
  if (np->kids)
    fprintf(stderr, "tree_graft: internal error: np->kids should be NULL\n");
  else
    {
      np->kids = tp->root->kids;
      tree_ns_merge(np->tree, tp->ns_used);
    }
}

void
node_iterator(Node *np, void *user, void (*nodefnc)(Node *np, void *user), void (*postfnc)(Node *np, void *user))
{
  _do_node(np, user, nodefnc, postfnc);
}

/* Replace the contents of to node with from node, but keep the
   linkages of the to node */
void
node_replace(Node *from, Node *to)
{
  to->ns = from->ns;
  to->name = from->name;
  to->text = from->text;
  to->kids = from->kids;
  to->props = from->props;
  to->user = from->user;
  /* leave other structure members because they relate to location in
     file/tree */
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
nodeh_register(nodehandlers nh, nscode nt, nodehandler fnc)
{
  nh[nt] = fnc;
}
