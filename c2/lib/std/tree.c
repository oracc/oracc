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
tree_add(Tree *tp, const char *name, int depth, Mloc *loc)
{
  Node *np = NULL;
  
  if (tp->curr->kids)
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
      np->rent = tp->curr;
      tp->curr->last = tp->curr->kids = np;
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
