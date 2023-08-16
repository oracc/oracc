#include <oraccsys.h>
#include <signlist.h>
#include <sx.h>

#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

static Hash *usigns;
static const char *sx_unicode_rx_mangle(struct sl_signlist *sl, const char *g);

static int cmp_by_len(const void *a, const void *b)
{
  return strlen(*(char**)b) - strlen(*(char**)a);
}

void
sx_unicode(struct sl_signlist *sl)
{
  usigns = hash_create(1024);
  /* Index the signs that have unames -- those are encoded so we treat
     them as atoms even if they are compounds */
  int i;  
  for (i = 0; i < sl->nsigns; ++i)
    {
      struct sl_unicode *Up = (sl->signs[i]->xref ? &sl->signs[i]->xref->U : &sl->signs[i]->U);
      if (Up->uname)
	{
	  if (Up->uhex)
	    hash_add(usigns, sl->signs[i]->name, (ucp)Up->uhex);
	  else
	    mesg_verr(&sl->signs[i]->inst->mloc, "sign %s has uname but no uhex\n", sl->signs[i]->name);
	}
    }

  /* Create a pattern to use for PCRE2 matching */
  int nk;
  const char **k = hash_keys2(usigns, &nk);
  qsort(k, nk, sizeof(const char *), cmp_by_len);

  List *kl = list_create(LIST_SINGLE);
  for (i = 0; i < nk; ++i)
    {
      if (i)
	list_add(kl, "|");
      list_add(kl, (void*)sx_unicode_rx_mangle(sl, k[i]));
    }

  unsigned const char *pat = list_concat(kl);

#if 0
  fputs((ccp)pat, stderr);
  fputc('\n', stderr);
#endif

  pcre2_code*cpat = pcre2_compile((PCRE2_SPTR)pat, strlen((ccp)pat),
				  PCRE2_UTF, NULL, NULL, NULL);
  if (cpat)
    {
    }
  else
    fprintf(stderr, "sx: pcre2_compile failed\n");
  
  /* check the compounds by building ucode sequences for them:
   *   if there is one in the signlist already, check that the newly built ucode is the same
   *   else set the sign's U.ucode appropriately
   */
  struct sl_inst *ip;
  for (ip = list_first(sl->compounds); ip; ip = list_next(sl->compounds))
    {
      unsigned const char *name = ip->type == 's' ? ip->u.s->name : ip->u.f->name;
      struct sl_unicode *Up = ip->type == 's' ? &ip->u.s->U : &ip->u.f->U;
      if (!Up->uhex)
	{
	  /*struct sl_token *tp = hash_find(sl->htoken, name);*/
	  if (Up->useq)
	    fprintf(stderr, "sx_unicode: checking @useq %s for %s\n", Up->useq, name);
	  else
	    fprintf(stderr, "sx_unicode: building @useq for %s\n", name);
	}
    }

  for (i = 0; i < sl->nsigns; ++i)
    {
      if (!sl->signs[i]->U.utf8)
	{
	  if (sl->signs[i]->U.uhex)
	    sl->signs[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.uhex), sl->p);
	  else if (sl->signs[i]->U.useq)
	    sl->signs[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->signs[i]->U.useq), sl->p);
	}
    }

  for (i = 0; i < sl->nforms; ++i)
    {
      if (!sl->forms[i]->U.utf8)
	{
	  if (sl->forms[i]->U.uhex)
	    sl->forms[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.uhex), sl->p);
	  else if (sl->forms[i]->U.useq)
	    sl->forms[i]->U.utf8 = pool_copy(uhex2utf8((uccp)sl->forms[i]->U.useq), sl->p);
	}
    }

}

/**Prepare a sign-name for use in pattern-matching:
 *
 *   - remove enclosing |...|
 *   - map . to , 
 *   - map + to ;
 *   - adding a ',' at beginning and end
 *
 * The mangled string is added as a hash key to usigns--there's no
 * possibility of conflict because of the bracketing with ,..., so 'A'
 * is mangled to ',A,'.
 *
 */
static const char *
sx_unicode_rx_mangle(struct sl_signlist *sl, const char *g)
{
  char *res = NULL;
  if ('|' == *g)
    {
      res = (char*)pool_alloc(strlen(g)+1, sl->p);
      ++g;
    }
  else
    {
      res = (char*)pool_alloc(strlen(g)+3, sl->p);
    }
  const char *src = g;
  char *dst = res;
  *dst++ = ',';
  while (*src)
    {
      if ('.' == *src)
	{
	  *dst++ = ',';
	  ++src;
	}
      else if ('+' == *src)
	{
	  *dst++ = ';';
	  ++src;
	}
      else if ('|' == *src)
	++src;
      else
	*dst++ = *src++;
    }
  *dst++ = ',';
  *dst = '\0';
  hash_add(usigns, (uccp)res, hash_find(usigns, (uccp)g));
  return res;
}

void
sx_unicode_table(FILE *f, struct sl_signlist *sl)
{
  const char **u = hash_keys(usigns);
  int i;
  for (i = 0; u[i]; ++i)
    fprintf(f, "%s\t%s\n", u[i], (char*)hash_find(usigns, (uccp)u[i]));
}
