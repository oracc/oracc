#include <stdlib.h>
#include <stdio.h>
#include "redblack.h"
#include "sortinfo.h"
#include "pg.h"

extern int csi_debug;
extern int nsortfields;
extern int *sortfields;
extern struct sortinfo *sip;

static int sic_id = 0;
static struct si_cache **siclist;

int
sic_cmp(const void *a, const void *b, const void *c)
{
  int i,max;
  for (i = 0,max=nsortfields-1; i < max; ++i)
    {
      int f = sortfields[i];
      if (((struct si_cache*)a)->codes[f] != ((struct si_cache*)b)->codes[f])
	return ((struct si_cache*)a)->codes[f] - ((struct si_cache*)b)->codes[f];
    }
  return 0;
}

int
sic_id_cmp(const void *a, const void *b)
{
  return (*(struct si_cache**)a)->id - (*(struct si_cache**)b)->id;
}

int
sic_pd_cmp(const void *a, const void *b)
{
  return (*(struct si_cache**)a)->codes[2] - (*(struct si_cache**)b)->codes[2];
}

/* resort the cache by period */
void
pd_sort_cache(void)
{
  qsort(siclist, sic_id, sizeof(struct si_cache *), sic_pd_cmp);
}

struct si_cache **
make_cache(struct item **items, int nitems, int*sic_size)
{
  struct rbtree *rb;
  RBLIST *rbl;
  struct si_cache *sicp;
  int i;
  static u4 top;

  sic_id = 0;

  /* No cache unless there are keys; NB: we use a cache
     even for 1 key because it simplifies X-processing */
  if (nsortfields == 0)
    return NULL;
  rb = rbinit(sic_cmp,NULL);

  top = sip->qoffset;

  /* cache the indexes into the sortinfo */
  for (i = 0; i < nitems; ++i)
    {
      static u4 id = -1, new_id;
      static int j = 0, last_j;

      if (0 == i)
	{
	  j = 0;
	  id = -1;
	}

      new_id = strtoul((const char *)items[i]->pq+1,NULL,10);
      if (*items[i]->pq == 'X')
	new_id += 2000000;

      if (id  == new_id)
	{
	  items[i]->skey = items[i-1]->skey;
	  items[i]->grp = items[i-1]->grp;
	  continue;
	}
      else
	id = new_id;
      /*      fprintf(stderr,"id=%ld\n",id);*/
      if (j <= sip->qoffset && items[i]->pq[0] == 'Q')
	{
	  /*  fprintf(stderr,"resetting j to %ld; top to %ld\n",sip->qoffset, sip->nmember); */
	  j = sip->qoffset;
	  top = sip->nmember;
	}

      for (last_j = j; j <= top; ++j)
	if (id == sip->idlist[j])
	  {
	    static struct si_cache sic_tmp;
	    struct si_cache *sic;
	    /* fprintf(stderr,"found id %ld at j=%ld\n",id,j); */
	    sic_tmp.codes = &sip->scodes[j*sip->nfields];
	    if (csi_debug)
	      dbg_show_codes("sic_tmp.codes",id,j,sic_tmp.codes);
	    if (NULL == (sic = (struct si_cache*)rbfind(&sic_tmp, rb)))
	      {
		sic = malloc(sizeof(struct si_cache));
		sic->codes = &sip->scodes[j*sip->nfields];
		sic->id = sic_id++;
		(void)rbsearch(sic, rb);
	      }
	    items[i]->grp = sic->id;
	    items[i]->skey = sip->scodes[(sip->nfields*j++) + sortfields[nsortfields-1]];
	    break;
	  }
      if (j > top)
	{
	  fprintf(stderr,"pg: member %lu not in sortinfo\n", (unsigned long)id);
	  items[i]->skey = 0;
	  items[i]->grp = 0;
	  j = last_j;
	  /* fprintf(stderr,"resetting j to %ld from last_j\n", j); */
	  /* exit(1); */
	}
    }
  
  /* create an array that can be indexed during sorting and grouping */
  siclist = malloc(sic_id * sizeof(struct si_cache*));
  rbl = rbopenlist(rb);
  for (i = 0; NULL != (sicp = (struct si_cache*)rbreadlist(rbl)); ++i)
    {
      sicp->seq = i;
      siclist[i] = sicp;
    }
  rbcloselist(rbl);

  if (csi_debug)
    {
      fprintf(stderr, "#cache before qsort by ID\n");
      dbg_dump_sicache(siclist,sic_id);
    }

  /* the list is now sorted by sequence, so resort it by id so that
     indexing from pdata elements works correctly */
  qsort(siclist, sic_id, sizeof(struct si_cache *), sic_id_cmp);

  if (csi_debug)
    {
      fprintf(stderr, "#cache after qsort by ID\n");
      dbg_dump_sicache(siclist,sic_id);
    }

  *sic_size = sic_id;
  return siclist;
}
