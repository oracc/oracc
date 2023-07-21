#include <unidef.h>
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

/*enum sxc_state { sxc_before , sxc_during , sxc_contained };*/

/* g:d text=× sets this to 1; it is reset to 0 by g:s unless a g:gp
   sets it to 2 in which case g:gp sets it to 0 at the end of the
   (...) */
int sxc_contained = 0;

static void sx_compound(struct sl_signlist *sl, Node *gdl, const char *oid);
static void sx_compound_node(Node *np, struct sl_signlist *sl, const char *oid, int *nth);

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
sx_compound_node(Node *np, struct sl_signlist *sl, const char *sname, int *nth)
{
  if (np)
    {
      if (!strcmp(np->name, "g:s") || !strcmp(np->name, "g:l"))
	{
	  struct sl_c_data *cdp = NULL;

	  last_g = np->text;
	  
	  if (ctrace)
	    fprintf(stderr, "ctrace: g:s %s nth=%d contained=%d\n", np->text, *nth, sxc_contained);

	  if (!(cdp = hash_find(c, (uccp)np->text)))
	    {
	      cdp = sx_cd_init();
	      hash_add(c, (uccp)np->text, cdp);
	    }

	  if (!hash_find(cdp->p, (uccp)sname))
	    hash_add(cdp->p, (uccp)sname, "");

	  if (!nth)
	    list_add(cdp->i, (void*)sname);

	  if (sxc_contained)
	    {
	      if (!(hash_find(cdp->c, (uccp)sname)))
		hash_add(cdp->c, (uccp)sname, "");
	      if (sxc_contained == 1)
		sxc_contained = 0;
	    }
	  
	  ++*nth;

	  /* don't process g:l/g:s kids because we have mods from s->text
	     and we don't currently index @g etc (might one day) */
	}
      else if (!strcmp(np->name, "g:d"))
	{
	  if (!strcmp(np->text, U_s_X_u8str))
	    {
	      if (ctrace)
		fprintf(stderr, "ctrace: g:d %s\n", np->text);
	      if (last_g)
		{
		  struct sl_c_data *cdp = NULL;
		  if (!(cdp = hash_find(c, (uccp)np->text)))
		    {
		      cdp = sx_cd_init();
		      hash_add(c, (uccp)np->text, cdp);
		    }
		  if (!hash_find(cdp->t, (uccp)last_g))
		    hash_add(cdp->t, (uccp)sname, "");
		}
	      sxc_contained = 1;
	    }
	}
      else if (!strcmp(np->name, "g:gp"))
	{
	  if (sxc_contained)
	    sxc_contained = 2;
	  for (np = np->kids; np; np = np->next)
	    sx_compound_node(np, sl, sname, nth);
	  sxc_contained = 0;
	}
      else if (!strcmp(np->name, "g:c") || !strcmp(np->name, "g:n"))
	{
	  for (np = np->kids; np; np = np->next)
	    sx_compound_node(np, sl, sname, nth);
	}
      else if (!strcmp(np->name, "g:m"))
	; /* ignore @g on |(LAK079&LAK079)@g| */
      else if (!strcmp(np->name, "g:r"))
	; /* ignore repetition node on numbers */
      else
	{
	  fprintf(stderr, "sx: internal error: gvl node type %s not handled\n", np->name);
	}
    }
}

static void
sx_compound(struct sl_signlist *sl, Node *gdl, const char *sname)
{
  int nth = 0;
  if (gdl && !strcmp(gdl->kids->name, "g:c"))
    {      
      struct sl_c_data *cdp = NULL;

      if (ctrace)
	fprintf(stderr, "ctrace: start %s\n", gdl->kids->text);

      last_g = NULL;

      sx_compound_node(gdl->kids, sl, sname, &nth);

      if (last_g)
	{
	  if (!(cdp = hash_find(c, (uccp)last_g)))
	    {
	      cdp = sx_cd_init();
	      hash_add(c, (uccp)last_g, cdp);
	    }
	  list_add(cdp->f, (void*)sname);
	}
      
    }
}
