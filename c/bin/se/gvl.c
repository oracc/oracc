/* Grapheme Validation Tool Kit */

#include <stdlib.h>
#include <stdio.h>

#include <npool.h>
#include <hash.h>
#include <ctype128.h>
#include <atf.h>
#include <memblock.h>

#include "gvl.h"

static gvl_i *sl = NULL; /* sl is always the head of the list of signlist datasets, not necessarily the current one */
static gvl_i *curr_sl = NULL; /* the sl that should be used for look up */
static int tsv = 0;
static int gvl_trace = 0;

const unsigned char *(*gvl_lookup)(unsigned const char *key);

static gvl_i *gvl_i_init_d(const char *name, Dbi_index *dbi);
static gvl_i *gvl_i_init_h(const char *name, Hash_table *h);
static void   gvl_i_term(const char *name);

#define ccp const char *
#define ucp unsigned char *
#define uccp unsigned const char *

#define g_v "v"
#define g_s "s"
#define g_n "n"
#define g_p "p"
#define g_c "cs"
#define g_q "qv"

static int
gnr(const char *g)
{
  return ((*g == 'n' || *g == 'N') && (g[1]=='(' || g[1]=='+')) ? '1' : 0;
}

static const char *
gvl_type(unsigned const char *g)
{
  if ((*g > 127 || isalpha(*g) || *g == '\'') && !gnr((const char *)g))
    {
      if (strchr((ccp)g,'('))
	{
	  while (*g)
	    if ('!' == *g)
	      return g_v;
	    else if ('(' == *g)
	      return g_q;
	    else
	      ++g;
	  abort(); /* can't happen */
	}
      else
	{
	  if (u_isupper(g))
	    {
	      g += mbtowc(NULL,(const char *)g,6);
	      while (*g)
		if (u_islower(g))
		  return g_v;
		else if (isdigit(*g) || '@' == *g || '~' == *g)
		  break;
		else
		  g += mbtowc(NULL,(const char *)g,6);
	      return g_s;
	    }
	  else
	    return g_v;
	}
    }
  else if (isdigit(*g) || *g == 'n' || *g == 'N')
    return g_n;
  else if (*g == '|')
    {
      register unsigned const char *e = g;
      while (*++e != '|')
	;
      if (*e == '(')
	return g_q;
      else
	return g_c;
    }
  else if (*g == '*' || *g == ':')
    return g_p;
  else
    return NULL;  
}

static unsigned const char *
gvl_lookup_d(unsigned const char *key)
{
  return sl_lookup_d(curr_sl->u.d,key);
}

static unsigned const char *
gvl_lookup_h(unsigned const char *key)
{
  return hash_find(curr_sl->u.h, (const unsigned char *)key);
}

gvl_i*
gvl_setup(const char *project, const char *name, int arg_tsv)
{
  gvl_i *ret = NULL;
  tsv = arg_tsv;
  if (tsv)
    {
      Hash_table *h = NULL;
      
      gvl_lookup = gvl_lookup_h;

      if ((h = sl_init_h(project, name)))
	ret = gvl_i_init_h(name, h);
      else
	fprintf(stderr, "sl: failed to open TSV %s/%s\n", (char *)project, (char*)name);
    }
  else
    {
      Dbi_index *dbi = NULL;

      gvl_lookup = gvl_lookup_d;

      if ((dbi = sl_init_d(project, name)))
	ret = gvl_i_init_d(name, dbi);
      else
	fprintf(stderr, "sl: failed to open DBI %s/%s\n", (char *)project, (char*)name);
    }

  ret->tsv = arg_tsv;

  return ret;
}
      
void
gvl_wrapup(const char *name)
{
  gvl_i_term(name);
}

/* Don't error here; caller must decide if not finding the gvl is an error or not */
gvl_i *
gvl_i_find(const char *name)
{
  gvl_i *tmp = sl;
  while (tmp)
    {
      if (!strcmp(tmp->n, name))
	return tmp;
      else
	tmp = tmp->next;
    }
  return NULL;
}

gvl_i *
gvl_i_init_(const char *name)
{
  gvl_i *p = NULL;

  if (!name)
    return NULL;
  
  if (!(p = calloc(1, sizeof(struct gvl_i))))
    return NULL;

  p->n = name;
  
  if (!sl)
    {
      sl = p;
      sl->prev = sl->next = NULL;
    }
  else
    {
      gvl_i *tmp = NULL;
      if ((tmp = gvl_i_find(name)))
	{
	  free(p);
	  curr_sl = tmp;
	  return tmp;
	}

      p->prev = NULL;
      p->next = sl;
      sl->prev = p;
      sl = p;
    }
  curr_sl = sl;
  sl->h = hash_create(1024);
  sl->m = mb_init(sizeof(gvl_g), 1024);
  sl->p = npool_init();

  if (!sl->h || !sl->p)
    return NULL;

  return sl;
}

gvl_i *
gvl_i_init_d(const char *name, Dbi_index *dbi)
{
  gvl_i *p = NULL;
  if (dbi)
    {
      p = gvl_i_init_(name);
      if (!p->u.d)
	p->u.d = dbi;
    }
  return p;
}

gvl_i *
gvl_i_init_h(const char *name, Hash_table *h)
{
  gvl_i *p = NULL;
  if (h)
    {
      p = gvl_i_init_(name);
      if (!p->u.h)
	p->u.h = h;
    }
  return p;  
}

void
gvl_i_term(const char *name)
{
  gvl_i *tmp = sl;
  int found = 0;
  while (tmp)
    {
      if (!strcmp(tmp->n, name))
	{
	  gvl_i *next = tmp->next;
	  if (tmp->tsv)
	    hash_free(tmp->u.h, NULL);
	  else
	    sl_term_d(tmp->u.d);
	  hash_free(tmp->h, NULL);
	  npool_term(tmp->p);
	  free(tmp);
	  sl = next;
	  ++found;
	  goto ret;
	}
      else
	tmp = tmp->next;
    }
 ret:
  if (!found)
    fprintf(stderr, "gvl_i named '%s' not found during gvl_i_term\n", name);
}

unsigned char *
gvl_tmp_key(const char *oid, const char *field)
{
  static char tmpkey[19]; /* oid is always length=8; longest field is "contained" */
  strcpy(tmpkey,oid);
  if (*field)
    {
      char *tk = tmpkey + strlen(tmpkey);
      *tk++ = ';';
      strcpy(tk,field);
    }
  return (ucp)tmpkey;
}

gvl_g *
gvl_validate(unsigned const char *g)
{
  gvl_g *gp = NULL;

  if (g)
    {
      if (gvl_trace)
	fprintf(stderr, "gvl_validate: called with g=%s\n", g);
      if (!(gp = hash_find(sl->h,g)))
	{
	  if (gvl_trace)
	    fprintf(stderr, "gvl_validate: %s not found in seen-hash\n", g);
	  gp = mb_new(sl->m);
	  gp->text = npool_copy(g, sl->p);
	  hash_add(sl->h, gp->text, gp);
	  unsigned const char *l = NULL;
	  if ((l = gvl_lookup(g)))
	    {
	      /* best case: g is a known sign or value */
	      gp->oid = (ccp)l;
	      gp->sign = gvl_lookup(gvl_tmp_key((ccp)l,""));
	      fprintf(stderr, "hello g=%s; oid=%s; sn=%s\n", g, gp->oid, gp->sign);
	    }
	  else
	    {
	      if (gvl_trace)
		fprintf(stderr, "gvl_validate: %s not found as sign or value\n", g);	      
	    }
	}
      else
	{
	  if (gvl_trace)
	    fprintf(stderr, "gvl_validate: %s was found in hash\n", g);
	}
    }
  
  return gp;
}

/*
	  if ((gp->type = gvl_type(g)))
	    {
	      const char *k = npool_alloc(strlen(g)+2, sl->p);
	      sprintf(k, "%s;%s", g, gp->type);
	      if ((l = sl_lookup(k)))
		{
		  fprintf(stderr, "hello g=%s; type=%s; lookup=%s\n", g, t, l);
		  gp->oid = l;
		}
	      else
		{
		  gp->mess = "unknown grapheme";
		}
	    }
	  else
	    {
	      gp->mess = "bad type";
	    }
*/

