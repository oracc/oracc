#include <string.h>
#include <ctype128.h>
#include <oraccsys.h>
#include <hash.h>
#include <list.h>
#include <pool.h>
#include <dbi.h>
#include <gutil.h>
#include "sll.h"

static int signindicator[256];

int sll_raw_output = 0;
int sll_trace = 0;
const char *oracc = NULL;
Pool *sllpool = NULL;
Hash *sll_sl = NULL;
Dbi_index *sll_db = NULL;

const char *correctedg = NULL;

void
sll_init(void)
{
  sllpool = pool_init();
}

void
sll_term(void)
{
  pool_term(sllpool);
  sll_set_sl(NULL);
}

unsigned const char *
sll_lookup(unsigned const char *key)
{
  if (sll_sl)
    return sll_lookup_t(key);
  else
    return sll_lookup_d(sll_db, key);
}

List *
sll_resolve(unsigned const char *g, const char *e, struct sllext *ep)
{
  List *r = NULL;
  if (ep)
    {
      correctedg = (ccp)pool_copy((uccp)sll_ext_check(g, ep->type),sllpool);
      r = ep->fnc((ccp)correctedg);
    }
  else
    {
      correctedg = (ccp)pool_copy((uccp)g,sllpool);
      r = sll_get_one((ccp)g);
    }
  if (list_len(r))
    return r;
  else
    {
      list_free(r, NULL);
      return NULL;
    }
}

unsigned const char *
sll_ext_check(unsigned const char *k, enum sll_t t)
{
  if (t == SLL_ID)
    {
      if (*k == 'o' && isdigit(k[1]))
	return k;
      else
	return sll_lookup(k);
    }
  else if (t == SLL_SN)
    {
      if (sll_has_sign_indicator(k))
	return k;
      else
	{
	  unsigned const char *oid = sll_lookup(k);
	  if (oid)
	    return sll_lookup(oid);
	  else
	    return NULL;
	}
    }
  else
    return k;
}

/* Every sign name has at least one of these uppercase letters--this
   is validated by sl-xml */
void
sll_init_si(void)
{
  if (!signindicator['A'])
    signindicator['A'] = 
      signindicator['E'] = 
      signindicator['I'] = 
      signindicator['U'] = 
      signindicator['F'] = 
      signindicator['N'] = 
      signindicator['O'] = 
      signindicator['P'] = 
      signindicator['S'] = 
      signindicator['X'] = 1;
}

void
sll_set_sl(Hash *sl)
{
  sll_sl = sl;
}

unsigned char *
sll_strip_pp(unsigned const char *g)
{
  unsigned char *no_p = (ucp)pool_copy((uccp)g,sllpool), *end;
  unsigned const char *orig = g;
  end = no_p;
  while (*g)
    if ('+' == *g)
      *end++ = '.', ++g;
    else if ('(' != *g && ')' != *g)
      *end++ = *g++;
    else if ('(' == *g)
      {
	if (g > orig && (isdigit(g[-1]) || 'n' == g[-1] || 'N' == g[-1]))
	  {
	    /* copy the contents of this (...) including the '(' and
	       ')' because they belong to a number grapheme */
	    int nesting = 0;
	    while (*g)
	      {
		*end++ = *g++;
		if (')' == *g)
		  {
		    if (nesting)
		      --nesting;
		    else
		      {
			*end++ = *g++;
			break;
		      }
		  }
		else if ('(' == *g)
		  ++nesting;
	      }
	  }
	else
	  ++g;
      }
    else
      ++g;
  *end = '\0';
#if 0
  if (strcmp((const char*)no_p,(const char*)orig))
    fprintf(stderr, "sll_strip_pp %s => %s\n", (const char *)orig, (const char *)no_p);
#endif
  return no_p;
}

int
sll_has_sign_indicator(unsigned const char *g)
{
  while (*g && !isspace(*g))
    {
      if (signindicator[*g])
	return 1;
      ++g;
    }
  return 0;
}

const char *
sll_is_signlist(const char *key)
{
  if (key)
    {
      char *x;
      const char *y;
      for (y = key; *y; ++y)
	if (isdigit(*y))
	  break;
      if (*y)
	{
	  int len = y - key;
	  struct sll_signlist *sl = NULL;
	  x = malloc(len + 1);
	  strncpy(x, key, len);
	  x[len] = '\0';
	  sl = sll_signlist(x, len);
	  free(x);
	  if (sl)
	    return sl->name;
	}
    }
  return NULL;
}

unsigned char *
sll_tmp_key(unsigned const char *key, const char *field)
{
  static char *tmpkey = NULL;
  static int tmpkey_size = 0;
  if (key)
    {
      char *semi = NULL;
      if (!tmpkey || (strlen((ccp)key) > tmpkey_size))
	{
	  tmpkey_size = 10 * strlen((ccp)key);
	  tmpkey = (char *)pool_alloc(tmpkey_size, sllpool);
	}
      strcpy(tmpkey,(ccp)key);
      if ((semi = strchr(tmpkey, ';')))
	*semi = '\0';
      if (*field)
	{
	  char *tk = tmpkey + strlen(tmpkey);
	  *tk++ = ';';
	  strcpy(tk,field);
	}
      return pool_copy( (ucp)tmpkey,sllpool);
    }
  else
    return NULL;
}

/* sl.tsv encodings homophones as an OID/INDEX pair; this routine
   decodes a value base plus INDEX into the base+subdigits
 */
unsigned char *
sll_v_from_h(const unsigned char *b, const unsigned char *qsub)
{
  if (b && qsub)
    {
      int qsub_i = atoi((ccp)qsub);
      if (qsub_i >= 0)
	{
	  unsigned char *ret = pool_alloc( strlen((ccp)b)+7,sllpool);
	  int tens = qsub_i / 10;
	  int unit = qsub_i % 10;
	  const char *tensp = NULL, *unitp = g_sub_of(unit);
	  strcpy((char *)ret, (const char *)b);
	  if (qsub_i)
	    {
	      if (tens && ((tensp = g_sub_of(tens))))
		strcat((char *)ret,tensp);
	      if (unitp)
		strcat((char *)ret,unitp);
	    }
	  else
	    strcat((char*)ret,"ₓ");
	  return ret;
	}
    }
  return NULL;
}

/* Take a sign and see if it has a value with the same base as the
   grapheme arg g */
unsigned char *
sll_try_h(const char *oid, unsigned const char *g)
{
  unsigned char *b = g_base_of(g);
  unsigned const char *h = sll_lookup(sll_tmp_key(b,"h"));
  unsigned char *p = NULL;
  if (h)
    {
      if ((p = (ucp)strstr((ccp)h, oid)))
	{
	  unsigned char *p2 = NULL, *p_end = (ucp)strchr((char*)p,' '), *p_slash = NULL;
	  if (p_end)
	    {
	      p2 = pool_alloc( (p_end-p) + 1,sllpool);
	      strncpy((char*)p2,(char*)p,p_end-p);
	      p2[p_end-p] = '\0';
	    }
	  else
	    {
	      p2 = pool_copy((uccp)p,sllpool);
	    }
	  if ((p_slash = (ucp)strchr((ccp)p2,'/')))
	    {
	      p = sll_v_from_h((uccp)b, (uccp)p_slash+1);
	      if (!p)
		{
		  fprintf(stderr, "sll: internal error in data: sll_from_h failed on %s\n", p_slash);
		  p = (ucp)"(null)";
		}
	    }
	  else
	    p = b;
	}
    }
  return p;
}

/* Return a string consisting of the sign names for each of the OIDs
   in the arg string */
unsigned char *
sll_snames_of(unsigned const char *oids)
{
  List *l = list_create(LIST_SINGLE);
  unsigned char *xoids = (ucp)pool_copy((uccp)oids,sllpool), *xoid, *x, *ret;
  x = xoids;
  while (*x)
    {
      xoid = x;
      while (*x && ' ' != *x)
	++x;
      if (*x)
	*x++ = '\0';
      list_add(l,(void*)sll_lookup(xoid));
    }
  ret = list_join(l, " ");
  list_free(l,NULL);
  return ret;
}
