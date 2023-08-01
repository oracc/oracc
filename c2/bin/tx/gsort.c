#include <stdio.h>
#include <stdlib.h>
#include <tree.h>
#include <memo.h>
#include <gutil.h>
#include <collate.h>
#include "gdl.h"
#include "gsort.h"

void gsort_node(Node *np, List *lp);

Memo *m_gsort_header = NULL;
Memo *m_gsort_item = NULL;
Pool *gspool = NULL;

void
gsort_init()
{
  m_gsort_header = memo_init(sizeof(Gsort), 1024);
  m_gsort_item = memo_init(sizeof(Gitem), 1024);
  gspool = pool_init();
  collate_init((ucp)"unicode");
}

void
gsort_term()
{
  memo_term(m_gsort_header);
  memo_term(m_gsort_item);
}

Gitem *
gsort_create(unsigned const char *g, unsigned const char *r)
{
  Gitem *gp = memo_new(m_gsort_item);

  gp->g = g;
  gp->b = pool_copy(g_base_of(g), gspool);
  gp->k = collate_makekey(pool_copy(gp->b, gspool));
  gp->x = g_index_of(g, gp->b);
  if (r)
    {
      if ('n' == *r)
	gp->r = 1000;
      else if ('N' == *r)
	gp->r = 2000;
      else
	gp->r = atoi((ccp)r);
    }
  else
    gp->r = -1;
  return gp;
}

void
gsort_node(Node *np, List *lp)
{
  switch (np->name[2])
    {
    case 'p':      
    case 's':
    case 'v':
    case 'd':
      list_add(lp, gsort_create((uccp)np->text, NULL));
      break;
    case 'n':
      list_add(lp, gsort_create((uccp)np->kids->next->text, (uccp)np->kids->text));
      break;
    case 'c':
    case 'g':
      {
	Node *npp;
	for (npp = np->kids; npp; npp = npp->next)
	  gsort_node(npp, lp);
      }
      break;
    default:
      fprintf(stderr, "[gsort] unhandled name in gsort_node %s\n", np->name);
      break;
    }
}

Gsort *
gsort(Tree *tp)
{
  if (tp && tp->root && tp->root->kids)
    {
      List *lp = list_create(LIST_SINGLE);
      Gsort *gs = memo_new(m_gsort_header);
      Node *np;
      for (np = tp->root->kids; np; np = np->next)
	gsort_node(np, lp);
      gs->n = list_len(lp);
      gs->g = (Gitem **)list2array(lp);
      list_free(lp, NULL);
      return gs;
    }
  return NULL;
}

void
gsort_show(Tree *tp)
{
  Gsort *gsp = NULL;
  int i;
  
  gsp = gsort(tp);
  if (gsp)
    {
      for (i = 0; i < gsp->n; ++i)
	{
	  Gitem *gip = gsp->g[i];
	  fprintf(stdout, "{g=%s; b=%s; k=\"%s\"; x=%d; r=%d}",
		  gip->g, gip->b, gip->k, gip->x, gip->r);
	}
      fputc('\n', stdout);
    }
}

int
gsort_cmp(void *v1, void *v2)
{
  Gsort *g1 = *(Gsort**)v1;
  Gsort *g2 = *(Gsort**)v2;
  int i, ret;
  
  for (i = 0; i < g1->n && i < g2->n; ++i)
    {
      /* if it's numeric force it to compare after other signs */
      if (g1->r > 0)
	{
	  if ((ret = g1->r - g2->r))
	    return ret;
	  else if ((ret = strcmp(g1->b, g2->b)))
	    return ret;
	  if ((ret = g1->x - g2->x))
	    return ret;
	}
      else if (g2->r > 0)
	{
	  if ((ret = g2->r - g1->r))
	    return ret;
	  else if ((ret = strcmp(g2->b, g1->b)))
	    return ret;
	  if ((ret = g2->x - g1->x))
	    return ret;
	}
      else
	{
	  /* compare grapheme base */
	  if ((ret = strcmp(g1->b, g2->b)))
	    return ret;
	  /* compare index */
	  if ((ret = g1->x - g2->x))
	    return ret;
	}
    }
  if (i < g2->n)
    return -1;
  else if (i < g1->n)
    return 1;

  /* straight comparison of the graphemes (or delimiters) */
  for (i = 0; i < g1->n && i < g2->n; ++i)
    if ((ret = strcmp(g1->g, g2->g)))
      return ret;
  
  return 0;
}
