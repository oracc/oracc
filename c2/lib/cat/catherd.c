#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pool.h>
#include <memo.h>
#include <tree.h>

#include "cat.h"

extern int cattrace;

extern Memo *catnode_mem;
extern Pool *catpool;

char *
cat_name(struct catchunk *cp, char **data)
{
  char *n = NULL;
  if (cp && '@' == *cp->text)
    {
      char *end = NULL;
      end = n = cp->text + 1;
      /* name is terminated by space or NULL */
      while (*end && (*end > 127 || !isspace(*end)))
	++end;
      if (isspace(*end))
	{
	  *end++ = '\0';
	  *data = end;
	}
      else
	{
	  char *tmp = (char*)pool_alloc((end - n) + 1, catpool);
	  strncpy(tmp, n, n - end);
	  tmp[n-end] = '\0';
	  n = tmp;
	  *data = end;
	}
      return n;
    }
  else
    return NULL;
}

/* return 1 if end matches current parent; 0 otherwise */
void
cat_end(Tree *tp, char *data)
{
  char *ename = data;

  while (isalnum(*data))
    ++data;
  if (isspace(*data))
    {
      *data++ = '\0';
      while (isspace(*data))
	++data;
    }
  else if (*data)
    {
      /* junk after data */
      fprintf(stderr, "cat_end: junk after data %s\n", data);
      *data = '\0';
    }

  while (1)
    {
      if (!strcmp(tp->curr->name, ename))
	{
	  /* matched @end tag */
	  /*state->end = 0;*/
	  tree_pop(tp);
	  return;
	}
      else
	{
	  /* mismatched @end tag */
	  if (!tree_pop(tp))
	    break;
	}
    }
  return;
}

Tree *
cat_herd(struct catchunk *ccp, struct catconfig *cfg)
{
  struct catinfo *cip = NULL, *head_cip = NULL;
  struct catchunk *cp = NULL;
  Tree *tp = tree_init();

  head_cip = cfg->chkname(cfg->head, strlen(cfg->head));
  tree_root(tp, cfg->head, head_cip->depth, NULL);
  
  for (cp = ccp; cp; cp = cp->next)
    {
      if (*cp->text)
	{
	  struct catnode *cn = memo_new(catnode_mem);
	  static char *data = NULL;
	  
	  if (cattrace)
	    fprintf(stderr, "cat_herd: depth before processing node: %d\n", tp->curr->depth);
	  cn->name = cfg->getname(cp, &data);
	  if (data)
	    while (isspace(*data))
	      ++data;
	  if (cn->name && (cip = cfg->chkname(cn->name, strlen(cn->name))))
	    {
	      Node *np = NULL;

	      switch (cip->rel)
		{
		case CI_PARENT:
		  /* always make cn the last child of curr */
		  np = tree_add(tp, cn->name, cip->depth, NULL);
		  np->data = data;
		  if (cattrace)
		    fprintf(stderr, "cat_herd: curr=%s@%d: adding parent %s@%d; data=%s\n",
			    tp->curr->name, tp->curr->depth,
			    cn->name, cip->depth, data);
		  if (cip->depth > tp->curr->depth)
		    tree_push(tp);
		  else if (cip->depth < tp->curr->depth)
		    {
		      while (cip->depth < tp->curr->depth)
			tree_pop(tp);
		    }
		  cip->parse(np, data);
		  break;
		case CI_CHILD:
		  /* always make cn the last child of curr */
		  np = tree_add(tp, cn->name, cip->depth, NULL);
		  np->data = data;
		  if (cattrace)
		    fprintf(stderr, "cat_herd: curr=%s@%d: adding child %s@; data=%s\n",
			    tp->curr->name, tp->curr->depth,
			    cn->name, data);
		  cip->parse(np, data);
		  break;
		case CI_END:
		  if (cattrace)
		    fprintf(stderr, "cat_herd: curr=%s@%d: adding end %s@; data=%s\n",
			    tp->curr->name, tp->curr->depth,
			    cn->name, data);
		  cat_end(tp, data);
		  break;
		}
	    }
	  else
	    {
	      /* unknown name error */
	    }
	  if (cattrace)
	    fprintf(stderr, "cat_herd: depth after processing node: %d\n", tp->curr->depth);
	}
      else
	{
	  /* This is a paragraph break; can do validation of missing @end ... here */
	  if (tp->curr->depth > 1)
	    {
	      fprintf(stderr, "cat_herd: unexpected blank line--only allowed between records\n");
	      do
		tree_pop(tp);
	      while (tp->curr->depth > 1);
	    }
	}
    }
  return tp;
}
