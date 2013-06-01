#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype128.h>
#include "sortinfo.h"
#include "pg.h"

extern int csi_debug;
static struct item**pdata = NULL;
struct si_cache **sicache = NULL;
int full_count_mode = 0;
int sic_size;
long ndata = 0;
int *sortfields;
int nsortfields;

struct si_cache **make_cache(struct item **items, int nitems, int *sic_size);

/* compare only by PQ id because we uniq the pitems array down to one
   entry per text */

static int
icmp(const void *a,const void *b)
{
  struct item *ap = *(struct item**)a;
  struct item *bp = *(struct item**)b;
  char tab[27] = "abcdefghijklmnoACrstuvwByz"; /* Tricky: Q > X > P ! */

  if (strcmp((const char*)ap->pq,(const char *)bp->pq))
    {
      char a1 = *(const char*)ap->pq, b1 = *(const char*)bp->pq;
      if (a1 == b1)
	return strcmp((const char*)ap->pq,(const char *)bp->pq);
      else
	return tab[(int)a1-'A'] - tab[(int)b1-'A'];
    }

  return ap->lkey - bp->lkey;
}

static int
pg_cmp(const void *a, const void *b)
{
  if ((*((struct item**)a))->grp == -1
      || (*((struct item**)b))->grp == -1)
    {
      if ((*((struct item**)a))->grp == -1)
	{
	  if ((*((struct item**)b))->grp == -1)
	    return 0;
	  else
	    return -1;
	}
      else
	return 1;
    }
  else if (!sicache[(*((struct item**)a))->grp]
	   || !sicache[(*((struct item**)b))->grp])
    {
      if (!sicache[(*((struct item**)a))->grp])
	{
	  if (!sicache[(*((struct item**)b))->grp])
	    return 0;
	  else
	    return -1;
	}
      else
	return 1;
    }
  if (sicache[(*((struct item**)a))->grp]->seq 
      != sicache[(*((struct item**)b))->grp]->seq)
    return sicache[(*((struct item**)a))->grp]->seq 
      - sicache[(*((struct item**)b))->grp]->seq;

  if ((*((struct item**)a))->skey != (*((struct item**)b))->skey)
    return (*((struct item**)a))->skey - (*((struct item**)b))->skey;

  return (*((struct item**)a))->lkey - (*((struct item**)b))->lkey;
}

#include "sk_lookup.c"

static void
set_sortkeys(const char *s)
{
  int nkeys = 1;
  const char *t = s;
  while (*t)
    if (*t++ == ',')
      ++nkeys;
  sortfields = malloc(sizeof(int)*(nkeys+1));
  for(t = s, nkeys = 0; *t; ++nkeys)
    {
      if (isdigit(*t))
	sortfields[nkeys] = atoi(t);
      else
	sortfields[nkeys] = sk_lookup(t);
      while (*t && ',' != *t)
	++t;
      if (',' == *t)
	++t;
    }
  nsortfields = nkeys;
  sortfields[nkeys] = -1;
}

static void
uniq_pdata(void)
{
  int i,dest;
  if (ndata <= 1)
    return;
  for (dest = i = 1; i < ndata; ++i)
    {
      if (strcmp((const char *)pdata[dest-1]->s,(const char *)pdata[i]->s))
	{
	  if (dest != i)
	    pdata[dest++] = pdata[i];
	  else
	    ++dest;
	}
    }
  ndata = dest;
}

struct item **
pg_sort(struct item*items, int *nitems, 
	const char *sortkeys)
{
  int i;

  ndata = *nitems;
  pdata = malloc(ndata*sizeof(struct item*));

  /* set the pdata ptrs */
  for (i = 0; i < ndata; ++i)
    pdata[i] = &items[i];
  
  /* presort by P/Q and ID so we can traverse the sortinfo without thrashing */
  qsort(pdata,ndata,sizeof(struct item*),icmp);

  if (csi_debug)
    {
      fprintf(stderr,"#post-icmp-sort item list: ID/group/skey\n");
      dbg_dump_pitems(pdata, *nitems);
    }

  /* this is a good time to uniq unless we want full counts */
  if (!full_count_mode)
    uniq_pdata();

  /* set up the sort controller */
  set_sortkeys(sortkeys);
  for (i = 0; i < nsortfields; ++i)
    if (sortfields[i] > sip->nfields)
      {
	fprintf(stderr,"pg: sort key %d must be < %d\n",
		sortfields[i],sip->nfields);
	exit(1);
      }

  sicache = make_cache(pdata,ndata,&sic_size);

  /* run the sort */
  qsort(pdata,ndata,sizeof(struct item*),pg_cmp);

  *nitems = ndata;
  return pdata;
}
