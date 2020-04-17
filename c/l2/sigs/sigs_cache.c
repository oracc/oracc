#include <stdio.h>
#include "sigs.h"
#include "ilem_form.h"
#include "memblock.h"
#include "npool.h"

/* N.B. IF YOU START USING THE CACHE AGAIN YOU NEED TO ZERO WORDSET PCT */

struct npool *sigs_cache_pool = NULL;
int sigs_cache_pool_refs = 0;

static struct sig const * const *sigs_cache_lookup(struct xcl_context *xcp, 
						   struct ilem_form *ifp, struct sigset *sp, int *nfinds);

struct siglook look_cache = { "sigs_cache", sigs_cache_lookup, sig_look_cache };

void
sigs_cache_init(struct sigset *sp)
{
#if 0
  /* This is old cache handling; these days we autoload missing languages and we have per-lang
     cache rather than using sp->file == "cache" */
  if (!sp->file)
    return;
  if (!strcmp((const char *)sp->file, "cache"))
    {
      fprintf(stderr, "sigs_cache: can't cache a cache\n");
      return;
    }
#endif
  if (!sigs_cache_pool)
    {
      sigs_cache_pool = npool_init();
      sigs_cache_pool_refs = 0;
    }
  ++sigs_cache_pool_refs;
  sp->cache = mb_new(sp->owner->mb_sigsets);
  sp->cache->project = sp->project;
  sp->cache->lang = sp->lang;
  sp->cache->core = sp->core;
  sp->cache->file = (const unsigned char *)"cache";
  sp->cache->fmem = NULL;
  sp->cache->lines = NULL;
  sp->cache->forms = hash_create(1);
  sp->cache->norms = hash_create(1);
  sp->cache->bigrams_hash = NULL;
  sp->cache->collos_hash = NULL;
  sp->cache->mdsets_hash = NULL;
  sp->cache->cache = NULL;
  sp->cache->bigrams = NULL;
  sp->cache->psus = NULL;
  sp->cache->owner = sp->owner;
  sp->cache->loaded = 1;
}

void
sigs_cache_term(struct sigset *sp)
{
  hash_free(sp->cache->forms,NULL);
  hash_free(sp->cache->norms,NULL);
  if (--sigs_cache_pool_refs == 0)
    {
      if (sigs_cache_pool)
	{
	  npool_term(sigs_cache_pool);
	  sigs_cache_pool = NULL;
	}
    }
}

/* 
   fp->sp->owner is scp (assert this is non-NULL)
   fp->sp is sp
   sigs[0] is sig
 */
void
sigs_cache_add(struct ilem_form *ifp, struct sig const *const *sigs)
{
  return;

#if 0
  if (!ifp || !ifp->sp || BIT_ISSET(ifp->f2.flags, F2_FLAGS_FROM_CACHE))
    return;

  if (!ifp->sp->cache)
    sigs_cache_init(ifp->sp);

  if (verbose)
    fprintf(stderr, "sigs_cache: adding %s to cache\n", ifp->f2.form);

  sigs_load_one_sig(ifp->sp->owner, ifp->sp->cache, sigs[0]->sig, 0, ifp, 0);
#endif

#if 0
  if (BIT_ISSET(fp->f2.flags, F2_FLAGS_LEM_BY_NORM))
    hash_add(fp->sp->cache,
	     npool_copy(ifp->f2.norm, sigs_cache_pool),
	     (void*)sigs);
  else
    hash_add(fp->sp->cache,
	     npool_copy(ifp->f2.form, sigs_cache_pool),
	     (void*)sigs);
#endif
}

#if 0
/* FIXME: THIS SHOULD HANDLE NORM LEMMING AS WELL */
struct sig const * const *
sigs_cache_find(struct sigset *sp, struct ilem_form *fp)
{
  struct sig const * const *ret = NULL;

  if (verbose)
    fprintf(stderr, "sigs_cache: looking for %s in cache ... ", fp->f2.form);

  if (sp && sp->cache)
    {
      if ((BIT_ISSET(fp->f2.flags, F2_FLAGS_LEM_BY_NORM) && fp->f2.norm && (ret = hash_find(sp->cache->norms,fp->f2.norm)))
	  || (fp->f2.form && (ret = hash_find(sp->cache->forms,fp->f2.form))))
	{
	  if (verbose)
	    fprintf(stderr, "found\n");
	  return ret;
	}
    }

  if (verbose)
    fprintf(stderr, "not found\n");
  return NULL;
}
#endif

extern int lem_dynalem;

static struct sig const * const *
sigs_cache_lookup(struct xcl_context *xcp, struct ilem_form *ifp, struct sigset *sp, int *nfinds)
{
  if (!lem_dynalem && sp->cache)
    return sigs_inst_in_sigset(xcp, ifp, sp->cache, nfinds);
  else
    return NULL;
}

