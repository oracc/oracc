#include <stdio.h>
#include <stdlib.h>
#include <tree.h>
#include <memo.h>
#include <gutil.h>
#include <collate.h>
#include "gdl.h"
#include "gsort.h"

static GS_item *gsort_item(unsigned const char *n, unsigned const char *g, unsigned const char *r);
static void gsort_node(Node *np, List *lp);
static const char *gsort_show_key(unsigned const char *k);

static Memo *m_headers = NULL;
static Memo *m_items = NULL;
static Pool *gspool = NULL;
static Hash *hitems = NULL;
static Hash *hheads = NULL;

static GS_item gsort_null_item = { (uccp)"\\0" , (uccp)"", (uccp)"", (uccp)"", 0, -1 };

void
gsort_init()
{
  m_headers = memo_init(sizeof(GS_head), 1024);
  m_items = memo_init(sizeof(GS_item), 1024);
  gspool = pool_init();
  hitems = hash_create(1024);
  hheads = hash_create(1024);
  collate_init((ucp)"unicode");
}

void
gsort_term()
{
  memo_term(m_headers);
  memo_term(m_items);
  pool_term(gspool);
  hash_free(hheads, NULL);
  hash_free(hitems, NULL);
  gsort_show_key(NULL);
}

GS_head *
gsort_prep(Tree *tp)
{
  if (tp && tp->root)
    {
      GS_head *gs = memo_new(m_headers);

      if (tp->root->text && (gs = hash_find(hheads, (uccp)tp->root->text)))
	return gs;
      else
	{
	  List *lp = list_create(LIST_SINGLE);
	  gs = memo_new(m_headers);
	  if (tp->root->kids)
	    {
	      Node *np;
	      for (np = tp->root->kids; np; np = np->next)
		gsort_node(np, lp);
	      gs->n = list_len(lp);
	      gs->i = (GS_item **)list2array(lp);
	      list_free(lp, NULL);
	      if (tp->root->kids)
		gs->s = (uccp)tp->root->text;
	      hash_add(hheads, gs->s, gs);
	    }
	  else if (tp->root->text)
	    {
	      GS_item *gi = gsort_item((uccp)tp->root->text, (uccp)tp->root->text, NULL);
	      gs->n = 1;
	      gs->i = malloc(sizeof(GS_item*));
	      *gs->i = gi;
	      gs->s = (uccp)tp->root->text;
	      hash_add(hheads, gs->s, gs);
	    }
	  else
	    gs->s = NULL;
	  return gs;
	}
    }
  return NULL;
}

void
gsort_show(GS_head *gsp)
{
  if (gsp)
    {
      int i;
      fputs((ccp)gsp->s, stderr);
      fputc('\t', stderr);
      for (i = 0; i < gsp->n; ++i)
	{
	  GS_item *gip = gsp->i[i];
	  fprintf(stderr, "{%s; %s; %s; %s; %d; %d}",
		  gip->g, gip->b, gsort_show_key(gip->k), gip->m, gip->x, gip->r);
	}
      fputc('\n', stderr);
    }
  else
    fprintf(stderr, "(GS_head argument is NULL)\n");
}

static int
gsort_cmp_item(GS_item *a, GS_item *b)
{
  int ret = 0;

  /* force number graphemes to sort after other signs */
  if (a->r > 0 || b->r > 0)
    {
      if (a->r && b->r)
	{
	  /* for two numbers, compare via the grapheme so all AŠ, DIŠ,
	     etc. sort together */
	  /* compare grapheme base via the key */
	  if ((ret = strcmp((ccp)a->k, (ccp)b->k)))
	    return ret;
	  /* 1(AŠ) sort before 2(AŠ) */
	  if (a->r > b->r)
	    return 1;
	  else if (b->r > a->r)
	    return -1;
	  
	  /* compare index */
	  return a->x - b->x;
	}
      else
	{
	  if (a->r > b->r)
	    return 1;
	  else if (b->r > a->r)
	    return -1;
	}
    }

  /* compare grapheme base via the key */
  if ((ret = strcmp((ccp)a->k, (ccp)b->k)))
    return ret;
  
  /* compare index */
  if (a->x - b->x)
    return a->x - b->x;

  /* final check is mods */
  return strcmp((ccp)a->m, (ccp)b->m);
}

int
gsort_cmp(const void *v1, const void *v2)
{
  GS_head *h1 = *(GS_head**)v1;
  GS_head *h2 = *(GS_head**)v2;
  int i, ret;
  
  for (i = 0; i < h1->n && i < h2->n; ++i)
    if ((ret = gsort_cmp_item(h1->i[i], h2->i[i])))
      return ret;

  /* If all the members compared equal, the longer is later in sort order */
  return h1->n - h2->n;
}

static GS_item *
gsort_item(unsigned const char *n, unsigned const char *g, unsigned const char *r)
{
  GS_item *gp = NULL;
  unsigned char *tmp;

  if ((gp = hash_find(hitems, n)))
    return gp;

  gp = memo_new(m_items);
  gp->g = n;
  tmp = pool_copy(g_base_of(g), gspool);
  gp->b = tmp;
  gp->x = g_index_of(g, gp->b);

  if ((tmp = (ucp)strpbrk((ccp)gp->b, "~@")))
    {
      *tmp++ = '\0';
      gp->m = tmp;
    }
  else
    gp->m = (ucp)"";

  gp->k = collate_makekey(pool_copy(gp->b, gspool));

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

  hash_add(hitems, n, gp);
  
  return gp;
}

static void
gsort_node(Node *np, List *lp)
{
  /* Ignore determinatives for now; should probably ensure that forms
     with determinative sort in constant order */
  if (!strcmp(np->name, "g:det"))
    return;

  switch (np->name[2])
    {
    case 'l':
    case 'p':      
    case 's':
    case 'v':
    case 'd':
      list_add(lp, gsort_item((uccp)np->text, (uccp)np->text, NULL));
      break;
    case 'n':
      list_add(lp, gsort_item((uccp)np->text,
			      np->kids ? (uccp)np->kids->next->text : (uccp)np->text,
			      np->kids ? (uccp)np->kids->text : (uccp)np->text));
      break;
    case 'a':
    case 'm':
      list_add(lp, &gsort_null_item);
      list_add(lp, gsort_item((uccp)np->text, (uccp)np->text, NULL));
      break;
    case 'b':
    case 'c':
    case 'g':
      {
	Node *npp;
	for (npp = np->kids; npp; npp = npp->next)
	  gsort_node(npp, lp);
      }
      break;
    case 'q':
      /* for qualified signs use the value, a NULL delimiter (which
	 will sort before other delimiters) and the sign */
      list_add(lp, gsort_item((uccp)np->text, (uccp)np->kids->text, NULL));
      list_add(lp, &gsort_null_item);
      {
	Node *npp;
	for (npp = np->kids->next; npp; npp = npp->next)
	  gsort_node(npp, lp);
      }
      break;
    default:
      fprintf(stderr, "[gsort] unhandled name in gsort_node %s\n", np->name);
      break;
    }
}

static const char *
gsort_show_key(unsigned const char *k)
{
  static char *kk = NULL;
  char *kp;
  static int kk_len = 0;
  int kk_need = 0;

  if (!k)
    {
      kk_len = 0;
      free(kk);
      return (kk = NULL);
    }

  kk_need = (2 * strlen((ccp)k))+1;

  if (kk_need > kk_len)
    kk = realloc(kk, 2*kk_need);
  
  for (kp = kk; *k;)
    {
      if (*k < 32)
	{
	  *kp++ = '\\';
	  *kp++ = (*k++ + 'a');
	}
      else
	*kp++ = *k++;
    }
  *kp = '\0';
  
  return kk;
}
