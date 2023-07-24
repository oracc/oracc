#include <unidef.h>
#include <signlist.h>
#include <sx.h>

int ctrace = 0;

const char *last_g;

enum sxc_type { sxc_initial, sxc_medial, sxc_final, sxc_container, sxc_contained };

/* g:d text=× sets this to 1; it is reset to 0 by g:s unless a g:gp
   sets it to 2 in which case g:gp sets it to 0 at the end of the
   (...) */
int sxc_container_active = 0;
static int sxc_nth = 0;
static void sx_compound(struct sl_signlist *sl, Node *gdl, const char *oid);
static void sx_compound_node(Node *np, struct sl_signlist *sl, const char *oid);

struct cpd_lists
{
  List *memb;
  List *initial;
  List *medial;
  List *final;
  List *container;
  List *contained;
};

static void sx_compound_digest(struct sl_signlist *sl, Hash *h);
static void sx_compound_digest_l_to_d(struct cpd_lists *clp, struct sl_compound_digest *dp);
static void sx_compound_digest_oid(struct cpd_lists *clp, Hash *h, const char *n, const char *oid);

static struct sl_compound *
sx_compound_init(struct sl_signlist *sl, Hash *h, const char *c)
{
  struct sl_compound *cdp = memo_new(sl->m_compounds);
  hash_add(h, (uccp)c, cdp);
  return cdp;
}

/* The list of compounds has been made while reading the input.
   Compounds are always registered under an sl_sign*: signs which only
   occur as forms are processed using the sl_sign* which is the host
   for the form-as-sign. */
void
sx_compounds(struct sl_signlist *sl)
{
  unsigned const char *n;

  /* We can't process these in sorted order because we can't guarantee
     that we have all the sortable tokens until after processing the
     compounds */
  for (n = list_first(sl->compounds); n; n = list_next(sl->compounds))
    {
      struct sl_sign *s = hash_find(sl->hsentry, n);
      sxc_nth = 0;
      sx_compound(sl, s->gdl, (ccp)s->name);
    }
}

static struct
sl_signlist *cmpsl = NULL;

static int sign_name_cmp(const void *a, const void *b)
{
  const char *cc1 = (*(char**)a);
  const char *cc2 = (*(char**)b);
  struct sl_sign *sa1 = hash_find(cmpsl->hsentry, (uccp)cc1);
  struct sl_sign *sb1 = hash_find(cmpsl->hsentry, (uccp)cc2);
  int a1 = sa1->sort;
  int b1 = sb1->sort;
  if (a1 < b1)
    return -1;
  else if (a1 > b1)
    return 1;
  else
    return 0;
}

void
sx_compound_digests(struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    if (sl->signs[i]->hcompounds)
      sx_compound_digest(sl, sl->signs[i]->hcompounds);
}

static void
sx_compound_digest_init_cl(struct cpd_lists *clp)
{
  clp->memb = list_create(LIST_SINGLE);
  clp->initial = list_create(LIST_SINGLE);
  clp->medial = list_create(LIST_SINGLE);
  clp->final = list_create(LIST_SINGLE);
  clp->container = list_create(LIST_SINGLE);
  clp->contained = list_create(LIST_SINGLE);
}

static void
sx_compound_digest_term_cl(struct cpd_lists *clp)
{
  list_free(clp->memb, NULL);
  list_free(clp->initial, NULL);
  list_free(clp->medial, NULL);
  list_free(clp->final, NULL);
  list_free(clp->container, NULL);
  list_free(clp->contained, NULL);
}

static void
sx_compound_digest_oid(struct cpd_lists *clp, Hash *h, const char *n, const char *oid)
{
  struct sl_compound *cpd = hash_find(h, (uccp)n);
  list_add(clp->memb, (void*)oid);
  if (cpd->initial_or_final == -1)
    list_add(clp->initial, (void*)oid);
  else if (cpd->initial_or_final == 1)
    list_add(clp->final, (void*)oid);
  if (cpd->medial)
    list_add(clp->medial, (void*)oid);
  if (cpd->container)
    list_add(clp->container, (void*)oid);
  if (cpd->contained)
    list_add(clp->contained, (void*)oid);
}

static void
sx_compound_digest_l_to_d(struct cpd_lists *clp, struct sl_compound_digest *dp)
{
  dp->memb = (const char **)list2array(clp->memb);
  dp->initial = (const char **)list2array(clp->initial);
  dp->medial = (const char **)list2array(clp->medial);
  dp->final = (const char **)list2array(clp->final);
  dp->container = (const char **)list2array(clp->container);
  dp->contained = (const char **)list2array(clp->contained);
}

/* The Hash* arg here is the sl_sign*'s hcompound hash */
static void
sx_compound_digest(struct sl_signlist *sl, Hash *h)
{
  const char **keys;
  int nkeys, i;
  static struct cpd_lists cl;
  struct sl_compound_digest *d;

  keys = hash_keys2(h, &nkeys);

  /* sort the compound names so that the lists we build are naturally in sorted order */
  cmpsl = sl;
  qsort(keys, nkeys, sizeof(const char*), sign_name_cmp);
  cmpsl = NULL;

  sx_compound_digest_init_cl(&cl);
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_sign *sp = hash_find(sl->hsentry, (uccp)keys[i]);
      sx_compound_digest_oid(&cl, h, keys[i], sp->oid);
    }

  d = memo_new(sl->m_digests);
  sx_compound_digest_l_to_d(&cl, d);
  hash_add(h, (uccp)"#digest_by_oid", d);
  sx_compound_digest_term_cl(&cl);
}

static struct sl_sign *
sx_compound_new_sign(struct sl_signlist *sl, const char *sgnname, const char *cpdname)
{
  struct sl_sign *s = memo_new(sl->m_signs);
  struct sl_inst *ip = NULL;
  struct sl_form *fp = NULL;
  struct sl_sign *sp = NULL;
  
  if ((sp = hash_find(sl->hsentry, (uccp)cpdname)))
    ip = sp->inst;
  else if ((fp = hash_find(sl->hfentry, (uccp)cpdname)))
    ip = list_first(fp->insts);
  else
    fprintf(stderr, "sx: internal error: compound sign %s not found as sign or form signlist\n", cpdname);

  s->inst = ip;
  s->name = (uccp)sgnname;
  s->compound_only = 1;
  hash_add(sl->hsentry, s->name, s);
  asl_bld_token(sl, (uccp)sgnname);
  asl_register_sign(&ip->mloc, sl, s);
  return s;
}

static void
sx_compound_data(struct sl_signlist *sl, const char *sgnname, const char *cpdname, enum sxc_type t)
{
  struct sl_sign *sp = NULL;
  struct sl_compound *cdp = NULL;
  int new_sign = 0;

  sp = hash_find(sl->hsentry, (uccp)sgnname);

  if (!sp)
    {
      sp = sx_compound_new_sign(sl, sgnname, cpdname);
      new_sign = 1;
    }

  if (!sp->hcompounds)
    {
      sp->hcompounds = hash_create(32);
      if (ctrace)
	fprintf(stderr, "ctrace: adding data for compound %s under %ssign %s\n",
		cpdname, new_sign ? "new ": "", sgnname);
      cdp = sx_compound_init(sl, sp->hcompounds, cpdname);
    }
  else if (!(cdp = hash_find(sp->hcompounds, (uccp)cpdname)))
    {
      if (ctrace)
	fprintf(stderr, "ctrace: adding more data for compound %s under sign %s\n", cpdname, sgnname);
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

	  if (!sxc_nth)
	    sx_compound_data(sl, last_g, sname, sxc_initial);
	  else
	    sx_compound_data(sl, last_g, sname, sxc_medial);
	  /*list_add(cdp->i, (void*)sname);*/

	  if (sxc_container_active)
	    {
	      sx_compound_data(sl, last_g, sname, sxc_contained);
	      if (sxc_container_active == 1)
		sxc_container_active = 0;
	    }
	  
	  ++sxc_nth;

	  /* don't process g:l/g:s kids because we have mods from s->text
	     and we don't currently index @g etc (might one day) */
	}
      else if (!strcmp(np->name, "g:d"))
	{
	  if (!strcmp(np->text, U_X_u8str))
	    {
	      if (ctrace)
		fprintf(stderr, "ctrace: g:d %s\n", np->text);
	      if (last_g)
		{
		  sx_compound_data(sl, last_g, sname, sxc_container);
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

