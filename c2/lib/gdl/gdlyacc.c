#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tree.h>
#include "gdl.tab.h"
#include "gdl.h"

extern void gdl_wrapup_buffer(void);

Tree *
gdlparse_string(char *s)
{
  Tree *tp = tree_init();
  (void)tree_root(tp, "gdl:gdl", 1, NULL);
  gdl_setup_buffer(s);
  gdl_set_tree(tp);
  gdlparse();
  gdl_wrapup_buffer();
  return tp;
}

void
gdlparse_init(void)
{
  gdl_init();
}

void
gdlparse_reset(void)
{
}

void
gdlparse_term(void)
{
  gdl_term();
}

Node *
gdl_delim(Tree *ytp, const char *data)
{
  Node *np = NULL;
  fprintf(stderr, "DELIM: %c\n", '-');
  np = tree_add(ytp, "g:d", ytp->curr->depth, NULL); 
  np->data = data;
  return np;
}

Node *
gdl_graph(Tree *ytp, const char *data)
{
  Node *np = NULL;
  fprintf(stderr, "GRAPH: %s\n", gdllval.text);
  np = tree_add(ytp, "g:g", ytp->curr->depth, NULL);
  np->data = (ccp)pool_copy((uccp)data,gdlpool);
  return np;
}

void
gdl_prop(Node *ynp, int ptype, int gtype, const char *k, const char *v)
{
  if (ynp)
    ynp->props = prop_add(ynp->tree->propmem, ynp->props, ptype, gtype, k, v);
  else
    fprintf(stderr, "gdl_prop passed NULL ynp\n");
}

void
gdl_pop(Tree *ytp, const char *s)
{
  Node *np = tree_pop(ytp);
  if (!strcmp(np->name, s))
    /* error */;
}

void
gdl_push(Tree *ytp, const char *s)
{
  tree_add(ytp, s, ytp->curr->depth, NULL);
  tree_push(ytp);
}


#if 0
/* After refactoring the parser this became redundant but I'm keeping it around in case I ever want it again */
void
gdl_append(Node *ynp, const char *s)
{
  if (ynp)
    {
      if (ynp->data)
	{
	  char *p = (char*)pool_alloc(strlen(ynp->data)+strlen(s)+1, gdlpool);
	  (void)sprintf(p, "%s%s", ynp->data, s);
	  ynp->data = p;
	}
    }
  else
    {
      fprintf(stderr, "gdl_append passed NULL ynp\n");
    }
}
#endif
