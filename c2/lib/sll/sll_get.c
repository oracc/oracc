#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

#include <oraccsys.h>
#include <pool.h>
#include <dbi.h>
#include <gutil.h>
#include "sll.h"

static char *
sll_ret_pair(const char *k, const char *oid)
{
  char *p = NULL;
  if (k && oid)
    {
      p = (char*)pool_alloc(strlen(k) + strlen(oid) + 2, sllpool);
      (void)sprintf(p, "%s\t%s", k, oid);
    }
  else if (k)
    {
      p = (char*)pool_alloc(strlen(k) + 3, sllpool);
      (void)sprintf(p, "%s\t-", k);
    }
  else if (oid)
    {
      p = (char*)pool_alloc(strlen(oid) + 3, sllpool);
      (void)sprintf(p, "-\t%s", oid);
    }
  return p;
}

const char *
sll_get_name(unsigned char *k)
{
  const char *oid = (ccp)sll_lookup(k);
  if (oid)
    return sll_ret_pair((ccp)sll_lookup(sll_tmp_key((uccp)oid, "")), oid);
  else
    return NULL;
}

const char *
sll_get_oid(unsigned char *k)
{
  return (ccp)sll_ret_pair((ccp)k, (ccp)sll_lookup(k));
}

List *
sll_get_cpd(const char *k)
{
  List *r = NULL;
  const char *o = (ccp)sll_lookup(sll_tmp_key((uccp)k, wextension));
  if (o)
    {
      char *otok = (char*)pool_copy((uccp)o,sllpool);
      List *op = list_from_str(otok, NULL, LIST_SINGLE);
      char *o2 = NULL;

      r = list_create(LIST_SINGLE);
      for (o2 = list_first(op); o2; o2 = list_next(op))
	list_add(r, sll_ret_pair((ccp)sll_lookup((uccp)o2), o2));
    }
  return r;
}

List *
sll_get_one(const char *k)
{
  List *r = list_create(LIST_SINGLE);
  list_add(r, (void*)sll_lookup((uccp)k));
  return r;
}

List *
sll_get_one_let(const char *k)
{
  List *r = list_create(LIST_SINGLE);
  list_add(r, (void*)sll_lookup(sll_tmp_key((uccp)k,"let")));
  return r;
}

List *
sll_get_one_n(const char *k)
{
  List *r = list_create(LIST_SINGLE);
  list_add(r, (void*)sll_get_name((ucp)k));
  return r;
}

List *
sll_get_one_o(const char *k)
{
  List *r = list_create(LIST_SINGLE);
  list_add(r, (void*)sll_get_oid((ucp)k));
  return r;
}

List *
sll_get_forms(const char *k)
{
  List *r = NULL;
  const char *o = (ccp)sll_lookup(sll_tmp_key((uccp)k, "forms"));
  if (o)
    {
      char *otok = (char*)pool_copy((uccp)o,sllpool);
      List *op = list_from_str(otok, NULL, LIST_SINGLE);
      char *o2 = NULL;

      r = list_create(LIST_SINGLE);
      for (o2 = list_first(op); o2; o2 = list_next(op))
	{
	  char *tilde = strchr(o2, '/');
	  if (tilde)
	    {
	      unsigned const char *name = NULL, *sign = NULL;
	      char *form = NULL;
	      *tilde++ = '\0';
	      name = sll_lookup((uccp)o2);
	      if (!name)
		name = (uccp)pool_copy((uccp)sll_lookup(sll_tmp_key((uccp)o2, "name")), sllpool);
	      sign = sll_lookup((uccp)correctedg);
	      if (!sign)
		sign = sll_lookup(sll_tmp_key((uccp)correctedg, "name"));
	      form = (char*)pool_alloc(strlen((ccp)sign) + strlen((ccp)tilde) + 1, sllpool);
	      sprintf(form,"%s%s",sign,tilde);
	      list_add(r, sll_ret_pair((ccp)form, sll_ret_pair((ccp)name, (ccp)o2)));
	    }
	  else
	    list_add(r, sll_ret_pair((ccp)sll_lookup((uccp)o2), o2));
	}
    }
  return r;
}

List *
sll_get_h(const char *k)
{
  const char *h = (ccp)sll_lookup(sll_tmp_key((uccp)k, "h"));
  List *lp = NULL;
  if (h)
    {
      char *h2 = (char*)pool_copy((uccp)h,sllpool);
      List *hp = list_from_str(h2,NULL,LIST_SINGLE);
      const char *h1 = NULL;
      char *k2 = (char*)pool_copy((uccp)k,sllpool);
      char *b = k2, *bsemi;
      bsemi = strchr(b, ';'); if (bsemi) { *bsemi = '\0'; }
      lp = list_create(LIST_SINGLE);
      
      for (h1 = list_first(hp); h1; h1 = list_next(hp))
	{
	  char *s = strchr(h1,'/');	  
	  if (s)
	    {
	      unsigned const char *name = NULL;
	      *s++ = '\0';
	      name = sll_lookup((uccp)h1);
	      if (!name)
		name = sll_lookup(sll_tmp_key((uccp)h1, "name"));
	      list_add(lp, sll_ret_pair((ccp)name, sll_ret_pair((ccp)sll_v_from_h((uccp)b,(uccp)s), h1)));
	    }
	  else
	    {
	      unsigned const char *name = sll_lookup((uccp)h1);
	      if (!name)
		name = sll_lookup(sll_tmp_key((uccp)h1, "name"));
	      list_add(lp, sll_ret_pair((ccp)name, sll_ret_pair((ccp)b, h1)));
	    }
	}
    }
  return lp;
}
