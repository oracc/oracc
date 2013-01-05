#ifndef _PG_H
#define _PG_H

#include "psdtypes.h"

struct item
{
  unsigned char   *s;
  unsigned char   *pq;
  unsigned char   *qpq; /* qualified pq */
  Unsigned32   skey;
  Signed32 lkey;
  Signed32 grp;
};

struct outline
{
  struct si_cache *sic;
  char *hdr;
  Unsigned32 *poffsets;
  Unsigned32 *icounts;
  Signed32 page;
  Signed32 count;
};

struct page
{
  Signed32 used;
  char **p;
};

extern FILE*fdbg;
extern int csi_debug;
extern int sic_size;
extern int nsortfields, nheadfields;
extern int *sortfields, *headfields;
extern struct sortinfo *sip;
extern struct si_cache **sicache;

extern void dbg_dump_sicache(struct si_cache**cp, int sic_size);
extern void dbg_dump_pcache(struct si_cache**cpp, int nitems);
extern void dbg_dump_items(struct item*items, int nitems);
extern void dbg_dump_pitems(struct item**items, int nitems);
extern void dbg_dump_outline(struct outline *op, int nlevels);
extern void dbg_show_codes(const char *msg, int id, int idindex, s4*codes);

extern struct outline *pg_outline(int *nlevelsp);
extern void pg_outline_dump(FILE *fp, struct outline *o, int nlevels);
extern struct page *pg_page(struct item **pitems, int nitems, int *npages,
			    struct outline *outlinep);
extern void pg_page_dump_all(FILE *fp, struct page *pages, int npages);
extern void pg_page_dump_one(FILE *fp, struct page *p);
extern void pg_page_dump_zoomed(FILE *fp, struct item **items, int *nitems, int *npages, int zoomid, int zpage);

extern struct item **pg_sort(struct item*items, int*nitems, 
			     const char *sortkeys);
extern struct item *pg_union(int *nitems);
extern struct item *prune_p_with_q(struct item*,int*,int);

#endif /*_PG_H*/
