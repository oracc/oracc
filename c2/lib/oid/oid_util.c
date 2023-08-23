#include <stdlib.h>
#include <stdio.h>
#include <oraccsys.h>
#include <list.h>
#include <oid.h>

int oo_xids = 0;

/* The assignees, k, have already been checked by oid_wants so we know
   that anything without an id needs one */
List *
oid_assign(Oids *o, Oids *k)
{
  size_t i;
  List *w = list_create(LIST_SINGLE);
  for (i = 0; i < k->nlines; ++i)
    {
      if (!k->oo[i]->id)
	{
	  k->oo[i]->id = strdup(oid_next_oid(o));
	  list_add(w, k->oo[i]);
	}
    }
  return w;
}

const char *
oid_domainify(const char *d, const char *k)
{
  static char *buf = NULL;
  static int buflen = 0;
  if (!d && !k)
    {
      if (buf)
	{
	  free(buf);
	  buf = NULL;
	}
    }
  else
    {
      while (strlen(d)+strlen(k)+2 > buflen)
	buflen += 128;
      buf = realloc(buf, buflen);
      sprintf(buf, "%s:%s", d, k);
    }
  return buf;
}

struct oid_ok_pair *
oid_ok_pair(const char *oid, const char *key)
{
  struct oid_ok_pair *lp = calloc(1, sizeof(struct oid_ok_pair));
  lp->oid = oid;
  lp->key = key;
  return lp;
}

/* lp argument must be non-NULL */
struct oid_ok_pair *
oid_ok_pair_last(struct oid_ok_pair *lp)
{
  while (lp->next)
    lp = lp->next;
  return lp;
}

static int32_t
oid_next_idnum(Oids *o)
{
  int i;
  int32_t last = -1;
  for (i = 0; o->lines[i]; ++i)
    {
      int32_t this;
      this = strtol((const char *)&o->lines[i][1], NULL, 10);
      if (last >= 0) {
	if (this - last > 1)
	  return last + 1;
      }
      last = this;
    }
  return last;
}

const char *
oid_next_oid(Oids *o)
{
  static char buf[9];
  int32_t n = oid_next_idnum(o);
  sprintf(buf, "%c%07d", (oo_xids ? 'x' : 'o'), n);
  return buf;
}

List *
oid_wants(Oids *o, Oids *k)
{
  size_t i;
  List *w = list_create(LIST_SINGLE);
  for (i = 0; i < k->nlines; ++i)
    {
      if (!k->oo[i]->id)
	{
	  const char *dk = oid_domainify(k->oo[i]->domain, (ccp)k->oo[i]->key);
	  struct oid *ho = hash_find(o->h, (uccp)dk);
	  if (ho)
	    k->oo[i]->id = ho->id;
	  else
	    list_add(w, k->oo[i]);
	}
    }
  return w;
}

void
oid_write(FILE *fp, Oids*o)
{
  size_t i;
  for (i = 0; i < o->nlines; ++i)
    fprintf(fp, "%s\t%s\t%s\t%s\t%s\n",
	    o->oo[i]->id,
	    o->oo[i]->domain,
	    o->oo[i]->key,
	    o->oo[i]->type,
	    o->oo[i]->extension ? o->oo[i]->extension : "");
}
