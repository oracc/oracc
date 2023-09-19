#include <signlist.h>
#include <sx.h>

extern int ctrace;

/* This module turns each sign's hash of compound-names with struct
   sl_compound* values into a collection of OID lists each implemented
   as a const char ** */

/* This is a transient structure used to invert the compound-name hashes */
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
static void sx_compound_digest_init_cl(struct cpd_lists *clp);
static void sx_compound_digest_term_cl(struct cpd_lists *clp);

static struct sl_signlist *cmpsl = NULL;

void
sx_compound_digests(struct sl_signlist *sl)
{
  int i;
  for (i = 0; i < sl->nsigns; ++i)
    if (sl->signs[i]->hcompounds)
      sx_compound_digest(sl, sl->signs[i]->hcompounds);
}

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

/* The Hash* arg here is the sl_sign*'s hcompound hash */
static void
sx_compound_digest(struct sl_signlist *sl, Hash *h)
{
  const char **keys;
  int nkeys, i;
  static struct cpd_lists cl;
  struct sl_compound_digest *d;

  keys = hash_keys2(h, &nkeys);

  if (ctrace)
    fprintf(stderr, "sx_compound_digest: processing %d keys\n", nkeys);
  
  /* sort the compound names so that the lists we build are naturally in sorted order */
  cmpsl = sl;
  qsort(keys, nkeys, sizeof(const char*), sign_name_cmp);
  cmpsl = NULL;

  sx_compound_digest_init_cl(&cl);
  for (i = 0; i < nkeys; ++i)
    {
      struct sl_sign *sp = hash_find(sl->hsentry, (uccp)keys[i]);
      if (ctrace)
	fprintf(stderr, "sx_compound_digest: processing compound %s with sl_sign %s\n", keys[i], sp->name);
      sx_compound_digest_oid(&cl, h, keys[i], sp->oid);
    }

  d = memo_new(sl->m_digests);
  sx_compound_digest_l_to_d(&cl, d);
  hash_add(h, (uccp)"#digest_by_oid", d);
  sx_compound_digest_term_cl(&cl);
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
