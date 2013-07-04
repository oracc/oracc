#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "sortinfo.h"
#include "pg.h"

#if 0
extern char *strdup(const char *);
#endif

struct page *pages;
struct page *currpage;
int currpage_used;
int npage = 0;
int pagesize = 15;
extern int quick;

char *
fmthdr(short sic_index)
{
  static char buf[1024], *s;
  int i;
  strcpy(buf,"#");
  for (i = 0; i < nheadfields; ++i)
    {
      u4*pindex = sip->pindex + (sicache[sic_index]->codes - sip->scodes);
      u4 poff = pindex[headfields[i]];
      if (csi_debug)
	fprintf(stderr,"using pool offset %d for %s\n",poff,&sip->pool[poff]);
      sprintf(buf+strlen(buf),"%s\t", &sip->pool[poff]);
    }
  buf[strlen(buf)-1] = '\0';
  for (s = buf; *s; ++s)
    if (' ' == *s)
      *s = '_';
  s = malloc(strlen(buf)+1);
  strcpy(s,buf);
  return s;
}

struct page *
pg_page(struct item **pitems, int nitems, int *npages, struct outline *outlinep)
{
  static int last_i = 0, i;

  /* page the results */
  npage = (nitems / pagesize) + (nitems % pagesize ? 1 : 0);
  pages = malloc(npage * sizeof(struct page));
  for (i = 0; i < npage; ++i)
    {
      pages[i].used = 0;
      pages[i].p = malloc((pagesize*2) * sizeof(char *));
    }
  currpage = pages;
  if (nheadfields)
    {
      currpage->p[0] = fmthdr(pitems[0]->grp);
      currpage->used = 1;
    }
  else
    currpage->used = 0;

  if (outlinep)
    outlinep->page = 1;
  
  for (i = 0; i < nitems; ++i)
    {
      if (!quick && i && pitems[i]->grp != pitems[i-1]->grp)
	{
	  if (outlinep)
	    outlinep->count = i - last_i;
	  last_i = i;
	  /* 	  if (currpage->used < pagesize) */
	  if (nheadfields)
	    currpage->p[currpage->used++] = fmthdr(pitems[i]->grp);
	  if (outlinep)
	    {
	      ++outlinep;
	      outlinep->page = 1 + currpage - pages;
	    }
	}
      if (currpage_used == pagesize)
	{
	  if (!quick && *currpage->p[currpage->used-1] == '#') {
	    --currpage->used;
	    if (outlinep)
	      (outlinep-1)->page++;
	  }
	  ++currpage;
	  currpage_used = 0;
	  if (nheadfields)
	    {
	      currpage->p[0] = fmthdr(pitems[i]->grp);
	      currpage->used = 1;
	    }
	  else
	    currpage->used = 0;
	}
      currpage->p[currpage->used++] = (char *)pitems[i]->s;
      ++currpage_used;
    }
  if (outlinep)
    outlinep->count = i - last_i;

  *npages = npage;
  return pages;
}

int
pg_find_page_with_id(struct page *pages, int npages, const char *id)
{
  int i;
  for (i = 0; i < npages; ++i)
    {
      struct page *p;
      int j;
      for (j = 0, p = &pages[i]; j < p->used; ++j)
	{
#if 1
	  if (!strcmp(p->p[j], id))
	    {
	      page_selector_index = j+1;
	      return i;
	    }
#else
	  char *colon = strchr(p->p[j], ':');
	  if (colon)
	    {
	      if (!strcmp(colon+1, id))
		{
		  page_selector_index = j+1;
		  return i;
		}
	    }
	  else
	    {
	      if (!strcmp(p->p[j], id))
		{
		  page_selector_index = j+1;
		  return i;
		}
	    }
#endif
	}
    }
  return 0;
}

void
pg_page_dump_all(FILE *fp, struct page *pages, int npages)
{
  int i;
  for (i = 0; i < npages; ++i)
    pg_page_dump_one(fp, &pages[i]);
}

void
pg_page_dump_one(FILE *fp, struct page *p)
{
  int i;
  for (i = 0; i < p->used; ++i)
    fprintf(fp,"%s ", p->p[i]);
  fputc('\n',fp);
}

void
pg_page_dump_zoomed(FILE *fp, struct item **items, int *nitems, int *npages, int zoomid, int zpage, const char *pg_sel_id)
{
  int i, j, zitems, zused;
  /* find the start of the range with this zoomid */
  for (i = 0; i < *nitems; ++i)
    if (items[i]->grp == zoomid)
      break;

  /* compute the number of pages */
  for (j = i; j < *nitems; ++j)
    if (items[j]->grp != zoomid)
      break;
  zitems = j - i;
  *npages = (zitems / pagesize) + ((zitems % pagesize) ? 1 : 0);
  *nitems = zitems;

  /* print the header */
  fprintf(fp,"%s ", fmthdr(zoomid));

  /* if we are finding page by the id, set zpage now */
  if (pg_sel_id)
    {
      int z;
      for (z = i; z< j; ++z)
	if (!strcmp((char*)items[i]->pq,pg_sel_id))
	  break;
      zpage = (z / pagesize) + ((z % pagesize) ? 1 : 0);
      page_selector_index = (z % pagesize)+1;
    }

  /* print the required page */
  for (i += ((zpage-1)*pagesize), zused = 0; i < j && zused++ < pagesize; ++i)
    fprintf(fp,"%s ", items[i]->s);
  fputc('\n',fp);
}

/*225958*/
