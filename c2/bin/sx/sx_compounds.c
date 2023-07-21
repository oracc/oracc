#include <signlist.h>
#include <sx.h>

int ctrace = 1;

/* keys are the individual signs */
static Hash *c;

/* values are where we gather the c-data; for each of the tracking
   lists/hashes we add the name of the compound sign to the tracker */
struct sl_c_data
{
  Hash *p; /* presence in compound; could occur multiple times so uniquing is required */
  List *i; /* initial: there can only be one initial in each compound so no hash needed */
  Hash *m; /* medial; could occur multiple times */
  List *f; /* final: there can only be one initial in each compound so no hash needed */
  Hash *t; /* times-container; could occur multiple times (reduplication) */
  Hash *c; /* contained sign; could occur multiple times */
};

Memo *cmem = NULL;

/* last grapheme identified in a single compound; used to set
   'final' */
const char *last_g = NULL;

enum sxc_state { sxc_before , sxc_during , sxc_contained };

static void sx_compound(struct sl_signlist *sl, Node *gdl, const char *oid);
static void sx_compound_node(Node *np, struct sl_signlist *sl, const char *oid, int nth);

static struct sl_c_data *
sx_cd_init(void)
{
  struct sl_c_data *cdp = memo_new(cmem);
  cdp->i = list_create(LIST_SINGLE);
  cdp->f = list_create(LIST_SINGLE);
  cdp->p = hash_create(3);
  cdp->m = hash_create(3);
  cdp->t = hash_create(3);
  cdp->c = hash_create(3);
  return cdp;
}

void
sx_compounds(struct sl_signlist *sl)
{
  unsigned const char *n;

  c = hash_create(1024);
  cmem = memo_init(sizeof(struct sl_c_data), 512);

  for (n = list_first(sl->compounds); n; n = list_next(sl->compounds))
    {
      struct sl_sign *s = hash_find(sl->hsentry, n);
      sx_compound(sl, s->gdl, (ccp)s->name);
    }
}

static void
sx_compound_node(Node *np, struct sl_signlist *sl, const char *sname, int nth)
{
  if (np)
    {
      if (!strcmp(np->name, "g:s"))
	{
	  struct sl_c_data *cdp = NULL;

	  last_g = np->text;
	  
	  if (ctrace)
	    fprintf(stderr, "ctrace: g:s %s nth=%d\n", np->text, nth);

	  if (!(cdp = hash_find(c, (uccp)np->text)))
	    {
	      cdp = sx_cd_init();
	      hash_add(c, (uccp)np->text, cdp);
	    }

	  if (!hash_find(cdp->p, (uccp)sname))
	    hash_add(cdp->p, (uccp)sname, cdp);

	  if (!nth)
	    list_add(cdp->i, (void*)sname);
	  
	  ++nth;
	}
      for (np = np->kids; np; np = np->next)
	sx_compound_node(np, sl, sname, nth);
    }
}

static void
sx_compound(struct sl_signlist *sl, Node *gdl, const char *sname)
{
  if (gdl && !strcmp(gdl->kids->name, "g:c"))
    {      
      struct sl_c_data *cdp = NULL;
      if (ctrace)
	fprintf(stderr, "ctrace: start %s\n", gdl->kids->text);
      last_g = NULL;
      sx_compound_node(gdl->kids, sl, sname, 0);
      if (!(cdp = hash_find(c, (uccp)last_g)))
	{
	  cdp = sx_cd_init();
	  hash_add(c, (uccp)last_g, cdp);
	}
      list_add(cdp->f, (void*)sname);
      
    }
}
