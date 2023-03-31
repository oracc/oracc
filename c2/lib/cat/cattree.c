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
	  int len = end - n;
	  char *tmp = (char*)pool_alloc(len + 1, catpool);
	  strncpy(tmp, n, len);
	  tmp[len] = '\0';
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
cat_tree(struct catchunk *ccp, struct catconfig *cfg)
{
  struct catinfo *cip = NULL, *head_cip = NULL;
  struct catchunk *cp = NULL;
  Tree *tp = tree_init();

  head_cip = cfg->chkname(cfg->head, strlen(cfg->head));
  tree_root(tp, cfg->ns, cfg->head, head_cip->depth, NULL);
  
  for (cp = ccp; cp; cp = cp->next)
    {
      if (*cp->text)
	{
	  /*struct catnode *cn = memo_new(catnode_mem);*/
	  static char *text = NULL;
	  char *name = NULL;
	  
	  if (cattrace)
	    fprintf(stderr, "cat_tree: depth before processing node: %d\n", tp->curr->depth);
	  name = cfg->getname(cp, &text);
	  if (text)
	    while (isspace(*text))
	      ++text;
	  if (name)
	    {
	      cip = cfg->chkname(name, strlen(name));
	      if (!cip)
		cip = cfg->chkname("#default", strlen("#default"));
	      if (cip)
		{
		  Node *np = NULL;
		  switch (cip->rel)
		    {
		    case CI_PARENT:
		      /* always make cn the last child of curr */
		      np = tree_add(tp, cfg->ns, name, cip->depth, NULL);
		      np->text = text;
		      np->mloc = mloc_file_line(cp->file, cp->line);
		      if (cattrace)
			fprintf(stderr, "cat_tree: curr=%s@%d: adding parent %s@%d; text=%s\n",
				tp->curr->name, tp->curr->depth,
				name, cip->depth, text);
		      if (cip->depth > tp->curr->depth)
			tree_push(tp);
		      else if (cip->depth < tp->curr->depth)
			{
			  while (cip->depth < tp->curr->depth)
			    tree_pop(tp);
			}
		      cip->parse(np, text);
		      break;
		    case CI_CHILD:
		      /* always make cn the last child of curr */
		      np = tree_add(tp, cfg->ns, name, cip->depth, NULL);
		      np->text = text;
		      np->mloc = mloc_file_line(cp->file, cp->line);
		      if (cattrace)
			fprintf(stderr, "cat_tree: curr=%s@%d: adding child %s@; text=%s\n",
				tp->curr->name, tp->curr->depth,
				name, text);
		      cip->parse(np, text);
		      break;
		    case CI_END:
		      if (cattrace)
			fprintf(stderr, "cat_tree: curr=%s@%d: adding end %s@; text=%s\n",
				tp->curr->name, tp->curr->depth,
				name, text);
		      cat_end(tp, text);
		      break;
		    }
		}
	      else
		{
		  fprintf(stderr, "cat_tree: name %s not in name-check table and no #default entry found\n", name);
		}
	    }
	  else
	    {
	      fprintf(stderr, "cat_tree: no name found in %s\n", text);
	    }
	  if (cattrace)
	    fprintf(stderr, "cat_tree: depth after processing node: %d\n", tp->curr->depth);
	}
      else
	{
	  /* This is a paragraph break; can do validation of missing @end ... here */
	  if (tp->curr->depth > 1)
	    {
	      if (!cfg->ignore_blanks)
		fprintf(stderr, "cat_tree: unexpected blank line--only allowed between records\n");
	      do
		tree_pop(tp);
	      while (tp->curr->depth > 1);
	    }
	}
    }
  return tp;
}
