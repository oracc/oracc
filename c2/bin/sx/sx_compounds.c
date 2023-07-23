#include <unidef.h>
#include <signlist.h>
#include <sx.h>

int ctrace = 1;

const char *last_g;

enum sxc_type { sxc_initial, sxc_medial, sxc_final, sxc_container, sxc_contained };

/* g:d text=× sets this to 1; it is reset to 0 by g:s unless a g:gp
   sets it to 2 in which case g:gp sets it to 0 at the end of the
   (...) */
int sxc_container_active = 0;
static int sxc_nth = 0;
static void sx_compound(struct sl_signlist *sl, Node *gdl, const char *oid);
static void sx_compound_node(Node *np, struct sl_signlist *sl, const char *oid);

static struct sl_compound *
sx_compound_init(struct sl_signlist *sl, Hash *h, const char *c)
{
  struct sl_compound *cdp = memo_new(sl->m_compounds);
  hash_add(h, (uccp)c, cdp);
  return cdp;
}

void
sx_compounds(struct sl_signlist *sl)
{
  unsigned const char *n;

  for (n = list_first(sl->compounds); n; n = list_next(sl->compounds))
    {
      struct sl_sign *s = hash_find(sl->hsentry, n);
      sxc_nth = 0;
      sx_compound(sl, s->gdl, (ccp)s->name);
    }
}

static void
sx_compound_data(struct sl_signlist *sl, const char *sgnname, const char *cpdname, enum sxc_type t)
{
  struct sl_sign *sp = NULL;
  struct sl_compound *cdp = NULL;

  sp = hash_find(sl->hsentry, (uccp)sgnname);
  if (!sp)
    {
      fprintf(stderr, "sx: internal error: sign %s not found in signlist\n", sgnname);
      return;      
    }
  if (!sp->hcompounds)
    {
      sp->hcompounds = hash_create(32);
      cdp = sx_compound_init(sl, sp->hcompounds, cpdname);
    }
  else if (!(cdp = hash_find(sp->hcompounds, (uccp)cpdname)))
    {
      cdp = sx_compound_init(sl, sp->hcompounds, cpdname);
    }

  if (ctrace)
    {
      const char *st = NULL;
      switch (t)
	{
	case sxc_initial:
	  st = "INITIAL";
	  break;
	case sxc_final:
	  if (cdp->medial == 1)
	    {
	      st = "FINAL (m=1 reset to 0)";
	      cdp->medial = 0;
	    }
	  else if (cdp->medial == 2)
	    {
	      st = "FINAL (m=2 reset to 1)";
	      cdp->medial = 1;
	    }
	  else
	    st = "FINAL (m=0)";
	  break;
	case sxc_container:
	  st = "CONTAINER";
	  break;
	case sxc_contained:
	  st = "CONTAINED";
	  break;
	case sxc_medial:
	  if (cdp->medial)
	    st = "MEDIAL/m";
	  else
	    st = "MEDIAL/s";
	  break;
	default:
	  abort();
	  break;
	}
      fprintf(stderr, "ctrace: add %s to sign %s in compound %s\n", st, sgnname, cpdname);
    }
  
  switch (t)
    {
    case sxc_initial:
      cdp->initial_or_final = -1;
      break;
    case sxc_final:
      cdp->initial_or_final = 1;
      break;
    case sxc_container:
      cdp->container = 1;
      break;
    case sxc_contained:
      cdp->contained = 1;
      break;
    case sxc_medial:
      if (cdp->medial)
	cdp->medial = 2;
      else
	cdp->medial = 1;
      break;
    default:
      abort();
      break;
    }
}

static void
sx_compound(struct sl_signlist *sl, Node *gdl, const char *sname)
{
  if (gdl && !strcmp(gdl->kids->name, "g:c"))
    {      
      if (ctrace)
	fprintf(stderr, "ctrace: start %s\n", gdl->kids->text);

      last_g = NULL;

      sx_compound_node(gdl->kids, sl, sname);

      if (last_g)
	sx_compound_data(sl, last_g, sname, sxc_final);
    }
}

static void
sx_compound_node(Node *np, struct sl_signlist *sl, const char *sname)
{
  if (np)
    {
      if (!strcmp(np->name, "g:s") || !strcmp(np->name, "g:l"))
	{

	  last_g = np->text;
	  
	  if (ctrace)
	    fprintf(stderr, "ctrace: g:s %s nth=%d contained=%d\n", np->text, sxc_nth, sxc_container_active);

#if 0
	  /* just use the keys of the sign's compounds hash to get list of compounds that sign is a cmemb of */
	  if (!hash_find(cdp->p, (uccp)sname))
	    hash_add(cdp->p, (uccp)sname, "");
#endif	  
	  if (!sxc_nth)
	    sx_compound_data(sl, last_g, sname, sxc_initial);
	  else
	    sx_compound_data(sl, last_g, sname, sxc_medial);
	  /*list_add(cdp->i, (void*)sname);*/

	  if (sxc_container_active)
	    {
#if 1
	      sx_compound_data(sl, last_g, sname, sxc_contained);
#else
	      if (!(hash_find(cdp->c, (uccp)sname)))
		hash_add(cdp->c, (uccp)sname, "");
#endif
	      if (sxc_container_active == 1)
		sxc_container_active = 0;
	    }
	  
	  ++sxc_nth;

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
#if 1
		  sx_compound_data(sl, last_g, sname, sxc_container);
#else
		  struct sl_c_data *cdp = NULL;
		  if (!(cdp = hash_find(c, (uccp)np->text)))
		    {
		      cdp = sx_cd_init();
		      hash_add(c, (uccp)np->text, cdp);
		    }
		  if (!hash_find(cdp->t, (uccp)last_g))
		    hash_add(cdp->t, (uccp)sname, "");
#endif
		}
	      sxc_container_active = 1;
	    }
	}
      else if (!strcmp(np->name, "g:gp"))
	{
	  if (sxc_container_active)
	    sxc_container_active = 2;
	  for (np = np->kids; np; np = np->next)
	    sx_compound_node(np, sl, sname);
	  sxc_container_active = 0;
	}
      else if (!strcmp(np->name, "g:c") || !strcmp(np->name, "g:n"))
	{
	  for (np = np->kids; np; np = np->next)
	    sx_compound_node(np, sl, sname);
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

